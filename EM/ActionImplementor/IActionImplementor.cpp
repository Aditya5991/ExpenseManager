#include "EM/pch.h"
#include "IActionImplementor.h"
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
    Interface::Interface()
    {
    }

    // public
	Interface::~Interface()
	{
        for (auto &[cmdType, actionHandler] : m_ActionHandlers)
        {
            delete actionHandler;
            actionHandler = nullptr;
        }
	}

    // public
    void Interface::InitializeActionHandlers()
    {
        // clear the action handlers before assigning them again,
        m_ActionHandlers.clear();
    }

}