#include "pch.h"
#include "ActionImplementor.h"
#include "Account/Manager.h"
#include "Account/Account.h"
#include "Common/CommonEnums.h"
#include "Common/EnumAndStringConverter.h"
#include "CLIParser/CLIParser.h"
#include "DBHandler/Util.h"
#include "DBTable_Expense.h"
#include "DBTable_Category.h"
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

    // private
    void ActionImplementor::Initialize()
    {
        InitializeActionHandlers();
    }

    // private
    void ActionImplementor::InitializeActionHandlers()
    {
        // clear the action handlers before assiging them again,
        // as the same command can have different action handlers for different accounts.
        m_ActionHandlers.clear();

        if (em::account::Manager::GetInstance().IsUsingAllAccounts())
        {
            RegisterHandler<em::action_handler::cli::List_AllAccounts>(em::CmdType::List);
        }
        else
        {
            RegisterHandler<em::action_handler::cli::List>(em::CmdType::List);
            RegisterHandler<em::action_handler::cli::Add>(em::CmdType::Add);
            RegisterHandler<em::action_handler::cli::Remove>(em::CmdType::Remove);
        }

        RegisterHandler<em::action_handler::cli::Report>(em::CmdType::Report);
        RegisterHandler<em::action_handler::cli::SwitchAccount>(em::CmdType::SwitchAccount);
        RegisterHandler<em::action_handler::cli::AddCategory>(em::CmdType::AddCategory);
        RegisterHandler<em::action_handler::cli::CompareMonths>(em::CmdType::CompareMonths);
        RegisterHandler<em::action_handler::cli::GitPush>(em::CmdType::GitPush);
    }

    // public
    void ActionImplementor::OnAccountSwitched()
    {
        InitializeActionHandlers();
    }

    //public
    action_handler::ResultSPtr ActionImplementor::PerformAction(CmdType cmdType)
    {
        if (cmdType == CmdType::ClearScreen)
        {
            system("cls");
            return action_handler::Result::Create(StatusCode::Success);
        }

        if (cmdType == CmdType::Help)
        {
            StatusCode code = DisplayHelp();
            return action_handler::Result::Create(code);
        }

        auto actionHandler = GetActionHandler(cmdType);
        if (!actionHandler)
        {
            std::string accountName = em::account::Manager::GetInstance().GetCurrentAccount()->GetName();
            std::string cmdTypeStr = em::common::EnumAndStringConverter::ConvertCmdTypeEnumToString(cmdType);
            std::string message = std::format("\nCommand: '{}' is not supported for account: '{}'", cmdTypeStr, accountName);
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