#include "EM/pch.h"
#include "AddTags.h"
#include "EM/DatabaseManager.h"

#include "DBHandler/Table.h"
#include "Utilities/StringUtils.h"

namespace em::action_handler::cli
{

    ResultSPtr AddTags::Execute(
        const std::string& commandName,
        const std::unordered_set<std::string>& flags,
        const std::map<std::string, std::vector<std::string>>& options)
    {
        assert(commandName == "addTags");

        std::vector<std::string> tags = utils::string::SplitString(options.at("names").front(), ',');
        
        auto table = databaseMgr.GetTable("tags");
        
        for (const std::string& tag : tags)
        {
            db::Model model;
            model["name"] = tag;
            if (!table->Insert(model))
            {
                ERROR_LOG(ERROR_DB_INSERT_TAG, tag);
                return Result::Create(StatusCode::DBError, std::format(ERROR_DB_INSERT_TAG, tag));
            }
        }

        return Result::Success();
    }

}