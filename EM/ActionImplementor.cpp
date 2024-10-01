#include "pch.h"
#include "ActionImplementor.h"
#include "Account/Manager.h"
#include "Account/Account.h"
#include "Common/CommonEnums.h"
#include "Common/EnumAndStringConverter.h"
#include "CLIParser/CLIParser.h"
#include "DBHandler/Util.h"
#include "ActionHandlers/Cli/ActionHandlers.h"
#include "TextTable.h"
#include "Conditions.h"
#include "ReportHandler.h"
#include "Renderer_TextTable.h"
#include "ErrorMessages.h"

namespace em
{
    ActionImplementor* ActionImplementor::s_Instance = nullptr;

    // private
    ActionImplementor::ActionImplementor()
    {
    }

	ActionImplementor::~ActionImplementor()
	{
        for (auto handler : m_ActionHandlers)
        {
            delete handler.second;
            handler.second = nullptr;
        }
        delete s_Instance;
        s_Instance = nullptr;
	}

    // private
    void ActionImplementor::Initialize()
    {
        InitializeActionHandlers();
    }

    // private
    void ActionImplementor::InitializeActionHandlers()
    {
        // clear the action handlers before assigning them again,
        m_ActionHandlers.clear();

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
    action_handler::ResultSPtr ActionImplementor::PerformAction(CmdType cmdType)
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

    // private
    StatusCode ActionImplementor::DisplayHelp()
    {
        cliParser.DisplayHelp();
        return StatusCode::Success;
    }

    // public static
    void ActionImplementor::Create()
    {
        DBG_ASSERT(s_Instance == nullptr);
        s_Instance = new ActionImplementor();
        s_Instance->Initialize();
    }

    // public static
    ActionImplementor& ActionImplementor::GetInstance()
    {
        DBG_ASSERT(s_Instance != nullptr);
        return *s_Instance;
    }


}