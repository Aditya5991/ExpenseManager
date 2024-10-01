#include "EM/pch.h"
#include "AddAccount.h"
#include "EM/DatabaseManager.h"
#include "EM/Conditions.h"
#include "EM/ConfigManager.h"
#include "EM/Exceptions/General.h"

#include "DBHandler/Util.h"
#include "DBHandler/Table.h"
#include "Utilities/StringUtils.h"

namespace em::action_handler::cli
{

    ResultSPtr AddAccount::Execute(
        const std::string& commandName,
        const std::unordered_set<std::string>& flags,
        const std::map<std::string, std::vector<std::string>>& options)
    {
        assert(commandName == "addAccount");

        auto accountsTable = databaseMgr.GetTable("accounts");

        // validate if the account already exists
        const std::string& name = options.at("name").front();
        if (accountsTable->CheckIfExists("name", name))
            return Result::Create(StatusCode::AccountAlreadySelected, std::format(ERROR_ACCOUNT_ALREADY_SELECTED, name));

        db::Model model;
        model["name"] = name;
        model["description"] = options.contains("description") ? options.at("description").front() : "";

        if (!accountsTable->Insert(model))
        {
            ERROR_LOG(ERROR_DB_INSERT_ACCOUNT, model["name"].asString());
            return Result::Create(StatusCode::DBError, std::format(ERROR_DB_INSERT_ACCOUNT, name));
        }

        return Result::Success();
    }

}