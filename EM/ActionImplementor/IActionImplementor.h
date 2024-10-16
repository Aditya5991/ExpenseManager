#pragma once

#include "EM/ActionHandlers/IActionHandler.h"
#include "EM/Common/CommonEnums.h"
#include "EM/Exceptions/General.h"

namespace em::action_impl
{

    /**
    * Singleton class to execute the commands for cli.
    */
    class Interface
    {
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
        virtual action_handler::ResultSPtr PerformAction(CmdType cmdType) = 0;

        /**
        * Initializes the action handlers
        */
        virtual void InitializeActionHandlers() = 0;

        /**
		* Destructor
		*/
        virtual ~Interface();

    protected:
        Interface();
        Interface(const Interface&) = default;

        /**
        * Displays help for the cli commands.
        */
        virtual StatusCode DisplayHelp() = 0;

        /**
        * Helper function to get the corresponding ActionHandler.
        *
        * @params [in] type
        *       Type of command to get the action handler for.
        */
        em::action_handler::Interface* GetActionHandler(CmdType type) { return m_ActionHandlers[type]; }

    protected:

        /**
        * This function is used to register custom handlers for CmdType.
        *
        * @params [in] cmdType
        *       Command Type for which we are registering the handler.
        *
        * @template_params Handler
        *       Child class of IActionHandler, which will handle the command.
        */
        template<typename Handler>
        Interface& RegisterHandler(CmdType cmdType);

        /* DataStructure to manage the ActionHandlers for all the commands. */
        std::unordered_map<CmdType, em::action_handler::Interface*>  m_ActionHandlers;
    };

    template<typename Handler>
    Interface& Interface::RegisterHandler(CmdType cmdType)
    {
        if (m_ActionHandlers.find(cmdType) != m_ActionHandlers.end())
            throw em::exception::General("Cannot re-register action handler!");

        m_ActionHandlers[cmdType] = new Handler();
        return *this;
    }

}