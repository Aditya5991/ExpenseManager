#include "pch.h"
#include "EM/ActionImplementor.h"
#include "CLIParser/CLIParser.h"
#include "CLIParser/ValidCommand.h"
#include "CLIParser/Utils.h"
#include "EM/ActionHandlers/Cli/ActionHandlers.h"
#include "EM/DatabaseManager.h"
#include "EM/ConfigManager.h"
#include "EM/Exceptions/Config.h"
#include "EM/Exceptions/General.h"
#include "EM/Common/EnumAndStringConverter.h"
#include "EM/Account/Account.h"
#include "EM/Account/Manager.h"
#include "EM/Utils.h"
#include "JsonHelper/json.h"

#include "DBHandler/Table.h"

#include <fstream>
#include <filesystem>


#define DATABASE_FILE_NAME "expense.db"

void GetCommandString(std::string& commandStr, std::vector<std::string>& args)
{
    printf("\n\n>> ");
    if(commandStr.empty())
        std::getline(std::cin, commandStr);

    std::istringstream iss(commandStr);
    std::vector<std::string> v;
    std::string s;
    while (iss >> std::quoted(s))
        args.push_back(s);
}

void GetAccountNameInput(std::string& accountName)
{
    std::cout << "Account: ";
    std::getline(std::cin, accountName);
    accountName.erase(accountName.find_last_not_of(" ") + 1);
}

em::CmdType GetCmdType(const char* cmdString)
{

    if (strcmp(cmdString, CmdString_Cls) == 0)
        return em::CmdType::ClearScreen;

    if (strcmp(cmdString, CmdString_ClearScreen) == 0)
        return em::CmdType::ClearScreen;

    if (strcmp(cmdString, CmdString_Help) == 0)
        return em::CmdType::Help;

    if (strcmp(cmdString, CmdString_AddCategory) == 0)
        return em::CmdType::AddCategory;

    if (strcmp(cmdString, CmdString_Add) == 0)
        return em::CmdType::Add;

    if (strcmp(cmdString, CmdString_Update) == 0)
        return em::CmdType::Update;

    if (strcmp(cmdString, CmdString_Remove) == 0)
        return em::CmdType::Remove;

    if (strcmp(cmdString, CmdString_List) == 0)
        return em::CmdType::List;

    if (strcmp(cmdString, CmdString_Report) == 0)
        return em::CmdType::Report;

    if (strcmp(cmdString, CmdString_CompareMonths) == 0)
        return em::CmdType::CompareMonths;

    if (strcmp(cmdString, CmdString_SwitchAccount) == 0)
        return em::CmdType::SwitchAccount;

    if (strcmp(cmdString, CmdString_GitPush) == 0)
        return em::CmdType::GitPush;

    if (strcmp(cmdString, CmdString_AddTags) == 0)
        return em::CmdType::AddTags;

    return em::CmdType::Invalid;
}

em::CmdType GetCmdType(int argc, char** argv)
{
    if (argc < 2)
        return em::CmdType::Invalid;

    return GetCmdType(argv[1]);
}

em::CmdType GetCmdType(std::vector<std::string>& args)
{
    if (args.size() < 1)
        return em::CmdType::Invalid;

    return GetCmdType(args[0].c_str());
}

void InitializeCLI()
{
    try
    {
        std::filesystem::path cliConfigFilePath = em::utils::GetCliConfigFilePath();
        if (!std::filesystem::exists(cliConfigFilePath))
            throw em::exception::General(std::format("File does not exist: {}", cliConfigFilePath.string()));

        std::ifstream inputStream(cliConfigFilePath);

        Json::Value root;
        inputStream >> root;

        Json::Value& commandsJson = root["commands"];
        for(const Json::Value& commandJson : commandsJson)
        {
            const std::string& commandName = commandJson["name"].asString();
            cli::ValidCommand& validCommand = cliParser.RegisterCommand(commandName);

            // Set the parameters.
            if (commandJson.isMember("parameters"))
            {
                const Json::Value& parametersJson = commandJson["parameters"];
                for (const Json::Value& paramJson : parametersJson)
                {
                    const std::string& paramName = paramJson["name"].asString();
                    const std::string& optionType = paramJson.isMember("optionType") ? paramJson["optionType"].asString() : "";
                    const std::string& helperMessage = paramJson.isMember("helperMessage") ? paramJson["helperMessage"].asString() : "";
                    bool isMandatory = paramJson.isMember("isMandatory") ? paramJson["isMandatory"].asBool() : false;
                    int inOrderIndex = paramJson.isMember("inOrderIndex") ? paramJson["inOrderIndex"].asInt() : -1;
                    int numValues = paramJson.isMember("numValues") ? paramJson["numValues"].asInt() : 1;

                    validCommand.AddParameter(paramName,
                        {
                            cli::ValidCommand::ConvertStringToOptionType(optionType),
                            helperMessage,
                            isMandatory,
                            inOrderIndex,
                            numValues
                        });
                }
            }

            // Set the flags.
            if (commandJson.isMember("flags"))
            {
                const Json::Value& flagsJson = commandJson["flags"];
                for (const Json::Value& flagJson : flagsJson)
                {
                    const std::string& name = flagJson["name"].asString();
                    const std::string& helperMessage = flagJson["helperMessage"].asString();
                    validCommand.AddFlag(name, helperMessage);
                }
            }
        }
    }
    catch (std::exception& e)
    {
        printf("\nFailed to read cliConfig.json: %s", e.what());
    }
}

void InitializeActionImplementor()
{
    em::ActionImplementor::Create();
}

void InitializeDatabase()
{
    std::filesystem::path configPath = em::utils::GetDatabaseConfigFilePath();

    Json::Value root;
    std::ifstream ifs(configPath);

    ifs >> root;

    std::string databaseName = root["name"].asString();

    em::DatabaseManager::Create(databaseName.c_str());
    em::DatabaseManager::GetInstance().RunMigrations();
    em::DatabaseManager::GetInstance().RegisterExpenseTables();
}

void InitializeAccountManager()
{
    em::account::Manager::Create();

    std::string accountName;
    if (em::ConfigManager::GetInstance().HasDefaultAccount())
        accountName = em::ConfigManager::GetInstance().GetDefaultAccount();
    else
        GetAccountNameInput(accountName);

    if (!em::account::Manager::GetInstance().AccountExists(accountName))
        throw em::exception::Config(std::format("Invalid account : {}", accountName));

    printf("\nActive Account: %s", accountName.c_str());
    em::account::Manager::GetInstance().SetCurrentAccountName(accountName);
}

void Initialize()
{
    InitializeAccountManager();
    InitializeDatabase();
    InitializeCLI();
    InitializeActionImplementor();
}

int main(int argc, char** argv)
{
    std::vector<std::any> vec = { "aditya", 28 };

    try
    {
        Initialize();

        bool quit = false;
        while (!quit)
        {
            std::string commandStr;
            std::vector<std::string> args;
            GetCommandString(commandStr, args);

            if (commandStr.empty())
                continue;

            if (commandStr == "quit")
                break;

            if (!cliParser.Parse(args))
                continue;

            em::CmdType cmdType = GetCmdType(args);
            if (cmdType == em::CmdType::Invalid)
            {
                printf("\nInvalid Command: %s", args[0].c_str());
                continue;
            }

            em::action_handler::ResultSPtr actionResult = actionImpl.PerformAction(cmdType);
            if (actionResult->statusCode != em::StatusCode::Success)
            {
                if (actionResult->statusCode == em::StatusCode::DisplayHelp)
                    cliParser.DisplayHelp(commandStr);

                printf("\nERROR: %s", actionResult->message.c_str());
            }

            printf("\n=============================================================");
        }
    }
    catch (std::exception& e)
    {
        printf("\n%s", e.what());
    }

    printf("\n\n");
    std::system("pause");

    return 0;
}

