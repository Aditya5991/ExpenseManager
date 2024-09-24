#include "pch.h"
#include "CLIParser.h"
#include "ValidCommand.h"
#include "Utils.h"
#include "JsonHelper/json.h"

BEGIN_NAMESPACE_CLI


/*************** CLIParser ***************/

std::mutex CLIParser::s_Mutex;
CLIParser* CLIParser::s_Instance = nullptr;

CLIParser* CLIParser::GetInstance()
{
    if (s_Instance == nullptr)
    {
        std::lock_guard<std::mutex> lk(s_Mutex);
        if(s_Instance == nullptr)
            s_Instance = new CLIParser();
    }
    return s_Instance;
}

//public
ValidCommand& CLIParser::RegisterCommand(const std::string& commandName)
{
    return GetValidCommands().emplace_back(commandName);
}

//public
bool CLIParser::HasFlag(const std::string& flagName) const
{
    return m_Flags.find(flagName) != m_Flags.end();
}

//public 
bool CLIParser::HasParameter(const std::string& paramName) const
{
    return m_UserParameters.find(paramName) != m_UserParameters.end();
}

//public
bool CLIParser::Parse(int argc, char** argv)
{
    SetExeName(argv[0]);
        
    argv++; argc--; // skip the .exe name

    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i)
        args.push_back(argv[i]);

    return Parse(args);
}

// public
void CLIParser::GenerateErrorMessage(const std::string& commandName) const 
{
    ValidCommand validCommand;
    if (!GetValidCommand(commandName, validCommand))
    {
        printf("\nInvalid Command : %s", commandName.c_str());
        DisplayHelp();
        return;
    }

    std::ostringstream oss;

    oss << "\nUsage: \t" << commandName << " ";
    const auto& validParams = validCommand.GetParameters();
    for (const auto [propName, prop] : validParams)
    {
        if (!prop.IsMandatory)
            continue;
        
        oss << "<" << propName << ">";
        oss << " ";
    }

    oss << "<options...> <flags...> ";

    // optional params
    
    bool firstOptionalRead = false;
    for (const auto [propName, prop] : validParams)
    {
        if (prop.IsMandatory)
            continue;

        if (!firstOptionalRead)
        {
            oss << "\n";
            oss << "Optional Params:";
            firstOptionalRead = true;
        }

        oss << "\n\t -" << propName << "  \t\t";
        oss << "- " << prop.HelperMessage;
    }

    // flags
    const auto& flags = validCommand.GetFlags();
    if (flags.size())
    {
        oss << "\nFlags: ";

        for (const auto [name, helperMessage] : flags)
        {
            oss << "\n\t--" << name << "  \t\t";
            oss << "- " << helperMessage;
        }
    }

    oss << "\n--------------------------------------------------------------------";
    printf("\n%s", oss.str().c_str());
}

// public
void CLIParser::DisplayHelp(const std::string& commandName) const
{
    for (const ValidCommand& cmd : m_ValidCommands)
    {
        if(commandName.empty() || commandName.find(cmd.GetName()) != std::string::npos)
            GenerateErrorMessage(cmd.GetName());
    }
}

// public
std::string CLIParser::AsJson()
{
    Json::Value root;

    // Add the command name.
    root["command"] = m_CurrentCommandName;

    // Add the command options/parameters.
    Json::Value optionsJson(Json::ValueType::objectValue);
    for (const auto& iter : m_UserParameters)
    {
        const std::string& userParamName = iter.first;
        const UserParameter& param = iter.second;
        Json::Value userParamJson(Json::ValueType::arrayValue);
        for (int i = 0; i < param.GetNumValues(); ++i)
            userParamJson.append(param.GetValue(i));

        optionsJson[userParamName] = userParamJson;
    }
    root["options"] = optionsJson;

    // Add the flags.
    Json::Value flagsJson(Json::ValueType::arrayValue);
    for (const auto& iter : m_Flags)
        flagsJson.append(iter);
    root["flags"] = flagsJson;

    Json::FastWriter writer;
    std::string jsonOutputString = writer.write(root);
    return jsonOutputString;
}


// private
void CLIParser::Clear()
{
    m_Flags.clear();
    m_UserParameters.clear();
    m_CurrentCommandName = "";
}

//private
bool CLIParser::Parse(const std::vector<std::string>& args)
{
    if (args.size() < 1)
        return false;

    Clear();

    m_CurrentCommandName = args[0];
    ValidCommand validCommand;
    if (!GetValidCommand(m_CurrentCommandName, validCommand) || !Validate(args, validCommand))
    {
        PrintHelperMessage();
        return false;
    }

    return true;
}

//private
bool CLIParser::ValidateArg(
    const std::string& userInput, 
    const ValidParameterProperties& paramProp) const
{
    bool isValid = true;

    switch (paramProp.Type)
    {
    case OptionType::TEXT:
        return true;

    case OptionType::DOUBLE:
        if (util::IsDouble(userInput))
            return true;

    case OptionType::ALPHA_NUMERIC:
        if (util::IsAlphaNumeric(userInput))
            return true;

    case OptionType::INTEGER:
        if (util::IsInteger(userInput))
            return true;

    case OptionType::DATE:
        if (util::IsDate(userInput))
            return true;
    }

    return false;
}

//private
bool CLIParser::Validate(
    const std::vector<std::string>& args, 
    const ValidCommand& validCommand)
{
    size_t numMandatoryOptions = validCommand.GetNumMandatoryParameters();

    size_t argIndex = 1;
    size_t mandatoryOptionIndex = 1;

    bool isValid = true;
    while (argIndex < args.size())
    {
        const std::string& currArg = args[argIndex];

        bool isOptionalParam = currArg[0] == '-' && currArg[1] != '-'; // '-' means an optional argument
        bool isFlag = (currArg[0] == '-' && currArg[1] == '-');
        if (isFlag) // is a flag
        {
            const std::string& option = args[argIndex];
            const std::string& flagName = currArg.substr(2, currArg.size() - 1); // remove the first '--' chars

            const auto& flags = validCommand.GetFlags();
            if (!validCommand.HasFlag(flagName))
            {
                isValid = false;
                break;
            }

            m_Flags.insert(flagName);
        }
        else if(isOptionalParam)
        {
            const std::string& userParamName = currArg.substr(1, currArg.size() - 1); // remove the first '-' char
            UserParameter userParameter(userParamName);

            const cli::ValidParamMap& validParams = validCommand.GetParameters();
            cli::ValidParamMap::const_iterator iter = validParams.find(userParamName);
            if (iter == validParams.end())
            {
                isValid = false;
                break;
            }

            const ValidParameterProperties& paramProp = iter->second;
            int numValues = paramProp.NumValues;

            for (int i = 0; i < numValues; ++i)
            {
                ++argIndex;
                const std::string& value = args[argIndex];
                if (!ValidateArg(value, paramProp))
                {
                    isValid = false;
                    break;
                }
                userParameter.AddValue(value);
            }

            m_UserParameters.insert(std::make_pair(userParamName, std::move(userParameter)));
        }
        else // Mandatory Parameter
        {
            if (mandatoryOptionIndex > numMandatoryOptions) // more mandatory args are passed
            {
                isValid = false;
                break;
            }

            const std::string& option = args[argIndex];

            auto iter = validCommand.GetParameterAtIndex(mandatoryOptionIndex);
            const ValidParameterProperties& paramProp = iter->second;
            if (!ValidateArg(option, paramProp))
                break;

            const std::string& userParamName = iter->first;
            UserParameter userParameter(userParamName);
            userParameter.AddValue(option);
            m_UserParameters.insert(std::make_pair(userParamName, std::move(userParameter)));

            ++mandatoryOptionIndex;
        }

        ++argIndex;
    }

    // check if all mandatory arguments were passed
    // as 'mandatoryOptionIndex' starts with 1, need to check mandatoryOptionIndex - 1 for the count of mandatory options passed by user.
    if (mandatoryOptionIndex - 1 < numMandatoryOptions)
    {
        // log the missing parameter
        for (size_t i = 1; i < numMandatoryOptions + 1; ++i)
        {
            const auto iter = validCommand.GetParameterAtIndex(i);
            const std::string& paramName = iter->first;

            if (!m_UserParameters.contains(paramName))
            {
                printf("\nMissing Parameter : %s", paramName.c_str());
                break;
            }
        }

        return false;
    }

    return isValid;
}

//private
bool CLIParser::GetValidCommand(const std::string& commandName, ValidCommand& validCommand) const
{
    for (const auto& vc : m_ValidCommands)
    {
        if (vc.GetName() == commandName)
        {
            validCommand = vc;
            return true;
        }
    }

    return false;
}

//private
void CLIParser::PrintHelperMessage()
{
    GenerateErrorMessage(m_CurrentCommandName);
}

/*****************************************/

END_NAMESPACE_CLI


