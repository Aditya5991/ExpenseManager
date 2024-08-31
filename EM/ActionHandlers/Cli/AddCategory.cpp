#include "../../pch.h"
#include "AddCategory.h"
#include "EM/DatabaseManager.h"
#include "EM/Conditions.h"

#include "DBHandler/Util.h"
#include "DBHandler/Table.h"

namespace em::action_handler::cli
{

    em::action_handler::ResultSPtr AddCategory::Execute(
        const std::string& commandName,
        const std::unordered_set<std::string>& flags,
        const std::map<std::string, std::string>& options)
    {
        assert(commandName == "addCategory");
        
        db::Model model;
        model["name"] = options.at("name");

        auto table = databaseMgr.GetTable("categories");
        bool isCategoryPresent = table->CheckIfExists("Name", model["name"].asString());
        if (!isCategoryPresent)
        {
            if (!table->Insert(model))
            {
                ERROR_LOG(ERROR_DB_INSERT_CATEGORY, model["name"].asString());
                return Result::Create(StatusCode::DBError, std::format(ERROR_DB_INSERT_CATEGORY, model["name"].asString()));
            }
        }
        else
        {
            ERROR_LOG(ERROR_CATEGORY_ALREADY_EXIST, model["name"].asString());
            return Result::Create(StatusCode::CategoryAlreayExists, std::format(ERROR_CATEGORY_ALREADY_EXIST, model["name"].asString()));
        }

        return Result::Create(StatusCode::Success);
    }

}