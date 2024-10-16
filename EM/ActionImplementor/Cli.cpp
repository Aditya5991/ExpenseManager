#include "EM/pch.h"
#include "EM/ActionImplementor/Cli.h"
#include "EM/Account/Manager.h"
#include "EM/Account/Account.h"
#include "EM/Common/CommonEnums.h"
#include "EM/Common/EnumAndStringConverter.h"
#include "EM/ActionHandlers/Cli/ActionHandlers.h"
#include "EM/TextTable.h"
#include "EM/Conditions.h"
#include "EM/ReportHandler.h"
#include "EM/Renderer_TextTable.h"
#include "EM/ErrorMessages.h"
#include "CLIParser/CLIParser.h"
#include "DBHandler/Util.h"

namespace em::action_impl
{
    // protected
    Cli::Cli()
    {
    }

    // public
	Cli::~Cli()
	{
        for (auto &[cmdType, actionHandler] : m_ActionHandlers)
        {
            delete actionHandler;
            actionHandler = nullptr;
        }
	}

    // public
    void Cli::InitializeActionHandlers()
    {
        base::InitializeActionHandlers();

        RegisterHandler<em::action_handler::cli::List>(em::CmdType::List);
        RegisterHandler<em::action_handler::cli::Add>(em::CmdType::Add);
        RegisterHandler<em::action_handler::cli::Update>(em::CmdType::Update);
        RegisterHandler<em::action_handler::cli::Remove>(em::CmdType::Remove);
        RegisterHandler<em::action_handler::cli::Report>(em::CmdType::Report);
        RegisterHandler<em::action_handler::cli::SwitchAccount>(em::CmdType::SwitchAccount);
        RegisterHandler<em::action_handler::cli::AddCategory>(em::CmdType::AddCategory);
        RegisterHandler<em::action_handler::cli::CompareMonths>(em::CmdType::CompareMonths);
        RegisterHandler<em::action_handler::cli::GitPush>(em::CmdType::GitPush);
        RegisterHandler<em::action_handler::cli::AddTags>(em::CmdType::AddTags);
        RegisterHandler<em::action_handler::cli::AddAccount>(em::CmdType::AddAccount);
        RegisterHandler<em::action_handler::cli::RemoveAccount>(em::CmdType::RemoveAccount);
    }

    //public
    action_handler::ResultSPtr Cli::PerformAction(CmdType cmdType)
    {
        if (cmdType == CmdType::ClearScreen)
        {
            system("cls");
            return action_handler::Result::Success();
        }

        if (cmdType == CmdType::Help)
        {
            StatusCode code = DisplayHelp();
            return action_handler::Result::Create(code);
        }

        auto actionHandler = GetActionHandler(cmdType);
        if (!actionHandler)
        {
            const std::string& accountName = em::account::Manager::GetInstance().GetCurrentAccountName();
            const std::string& cmdTypeStr = em::common::EnumAndStringConverter::ConvertCmdTypeEnumToString(cmdType);
            std::string message = std::format("\nNo Action Handler Registered for: '{}'", cmdTypeStr, accountName);
            return action_handler::Result::Create(StatusCode::CommandDoesNotExist, message);
        }

        em::action_handler::ResultSPtr result = actionHandler->Execute(cliParser.AsJson());
        return result;
    }

    // protected
    StatusCode Cli::DisplayHelp()
    {
        cliParser.DisplayHelp();
        return StatusCode::Success;
    }

}