#pragma once

#include "IActionImplementor.h"
#include "DBHandler/Database_SQLite.h"
#include "EM/ActionHandlers/IActionHandler.h"
#include "EM/Common/CommonEnums.h"
#include "EM/Exceptions/General.h"
#include "EM/App.h"
#include <EM/StatusCodes.h>

namespace em::action_impl
{

    /**
    * Singleton class to execute the commands for cli.
    */
    class Cli : public Interface
    {
        using base = em::action_impl::Interface;

    public:
        /**
        * This function handle the actual processing of the command.
        *
        * @params [in] cmdType
        *       Type of the command that will be executed
        *
        * @returns
        *       action_handler::ResultSPtr - representing the result of the function.
        */
        virtual action_handler::ResultSPtr PerformAction(CmdType cmdType) override;

        /**
        * Initializes the action handlers
        */
        virtual void InitializeActionHandlers() override;

        /**
		* Destructor
		*/
        ~Cli();

        Cli();
        Cli(const Cli&) = default;

        /**
        * Displays help for the cli commands.
        */
        virtual StatusCode DisplayHelp() override;
    };

}