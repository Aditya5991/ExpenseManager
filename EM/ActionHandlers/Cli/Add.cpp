#include "EM/pch.h"
#include "Add.h"
#include "EM/DatabaseManager.h"
#include "EM/Conditions.h"
#include "EM/ConfigManager.h"
#include "EM/Exceptions/General.h"
#include "EM/Account/Manager.h"
#include "DBHandler/Util.h"
#include "DBHandler/Table.h"
#include "Utilities/StringUtils.h"

namespace em::action_handler::cli
{

    ResultSPtr Add::Execute(
        const std::string& commandName,
        const std::unordered_set<std::string>& flags,
        const std::map<std::string, std::vector<std::string>>& options)
    {
        assert(commandName == "add");

        auto categoryTable = databaseMgr.GetTable("categories");
        auto expenseTable = databaseMgr.GetTable("expenses");

        db::Model model;
        // validate if the category exists
        db::Model categoryModel;
        const std::string& category = options.at("category").front();
        if (!categoryTable->CheckIfExists("name", category, &categoryModel))
            return Result::Create(StatusCode::CategoryDoesNotExist, std::format(ERROR_CATEGORY_DOES_NOT_EXIST, category));

        model["category_id"] = categoryModel["row_id"];
        model["name"] = options.at("name").front();
        model["price"] = std::stod(options.at("price").front());

        model["location"] = options.contains("location") ? options.at("location").front() : ConfigManager::GetInstance().GetDefaultLocation();

        if (flags.contains("yesterday"))
            model["date"] = db::DateTime::GetYesterdayDate().AsString();
        else
            model["date"] = options.contains("date") ? options.at("date").front() : db::DateTime::GetCurrentDate().AsString();

        if (options.contains("tags"))
        {
            std::string tag;
            if(!GenerateTags(options.at("tags").front(), tag))
                return Result::Create(StatusCode::DBError, std::format(ERROR_TAG_DOES_NOT_EXIST, options.at("tags").front()));

            model["tags"] = tag;
        }

        model["account"] = em::account::Manager::GetInstance().GetCurrentAccountName();

        if (!expenseTable->Insert(model))
        {
            ERROR_LOG(ERROR_DB_INSERT_EXPENSE, model["name"].asString());
            return Result::Create(StatusCode::DBError, std::format(ERROR_DB_INSERT_EXPENSE, model["name"].asString()));
        }

        return Result::Success();
    }

    // private
    bool Add::GenerateTags(const std::string& commaSeparatedTags, std::string& tagToInsert)
    {
        auto tagsTable = databaseMgr.GetTable("tags");

        // check if all tags exist in the database.
        std::vector<std::string> tags = utils::string::SplitString(commaSeparatedTags, ',');
        std::unordered_set<std::string> uniqueTags;
        for (const std::string& tag : tags)
        {
            if (!tagsTable->CheckIfExists("name", tag))
                return false;

            uniqueTags.insert(tag);
        }

        for (const std::string& tag : uniqueTags)
            tagToInsert += tag + ",";

        // remove last ','
        tagToInsert = tagToInsert.substr(0, tagToInsert.size() - 1);
        return true;
    }

}