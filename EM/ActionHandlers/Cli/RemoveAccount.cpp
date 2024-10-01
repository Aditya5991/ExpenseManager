#include "EM/pch.h"
#include "RemoveAccount.h"
#include "EM/DatabaseManager.h"
#include "EM/Conditions.h"
#include "EM/ConfigManager.h"
#include "EM/Exceptions/General.h"
#include "EM/Account/Manager.h"
#include "EM/Renderer_TextTable.h"
#include "DBHandler/Util.h"
#include "DBHandler/Table.h"
#include "Utilities/StringUtils.h"

namespace em::action_handler::cli
{

    ResultSPtr RemoveAccount::Execute(
        const std::string& commandName,
        const std::unordered_set<std::string>& flags,
        const std::map<std::string, std::vector<std::string>>& options)
    {
        assert(commandName == "removeAccount");
        return Result::GeneralFailure(ERROR_NOT_IMPLEMENTED);
        
        /**
        * SHOULD ENTRIES FOR THIS ACCOUNT BE DELETED AS WELL?
        */

        /*
        auto table = databaseMgr.GetTable("accounts");

        int rowId = std::stoi(options.at("row_id").front());

        db::Model row;
        if (!table->SelectById(row, rowId))
        {
            ERROR_LOG(ERROR_DB_REMOVE_ACCOUNT, rowId);
            return Result::Create(StatusCode::DBError, std::format(ERROR_DB_REMOVE_ACCOUNT, rowId));
        }

        const std::string& currentAccountName = em::account::Manager::GetInstance().GetCurrentAccountName();

        printf("\nFollowing rows will be deleted: ");
        Renderer_AccountTable::Render({ row });
        printf("\nProceed? : ");

        int userInput = 0;
        std::cin >> userInput;
        if (userInput == 0)
        {
            printf("\nSkipped deletion.");
            return Result::Success();
        }

        if (!table->DeleteById(rowId))
        {
            ERROR_LOG(ERROR_DB_REMOVE_ACCOUNT, rowId);
            return Result::Create(StatusCode::DBError, std::format(ERROR_DB_REMOVE_ACCOUNT, rowId));
        }

        return Result::Success();
        */
    }

}