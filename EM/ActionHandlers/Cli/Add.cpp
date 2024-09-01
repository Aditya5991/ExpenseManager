#include "EM/pch.h"
#include "Add.h"
#include "EM/DatabaseManager.h"
#include "EM/Conditions.h"
#include "EM/ConfigManager.h"
#include "EM/Exceptions/General.h"

#include "DBHandler/Util.h"
#include "DBHandler/Table.h"
#include "Utilities/StringUtils.h"

namespace em::action_handler::cli
{

    ResultSPtr Add::Execute(
        const std::string& commandName,
        const std::unordered_set<std::string>& flags,
        const std::map<std::string, std::string>& options)
    {
        assert(commandName == "add");

        auto validationResult = ValidateMandatoryArguments(flags, options);
        if (!validationResult->IsSuccess())
            return validationResult;

        auto categoryTable = databaseMgr.GetTable("categories");
        auto expenseTable = databaseMgr.GetTable(databaseMgr.GetCurrentExpenseTableName());

        db::Model model;
        // validate if the category exists
        model["category"] = options.at("category");
        if (!categoryTable->CheckIfExists("name", model["category"]))
            return Result::Create(StatusCode::CategoryDoesNotExist, std::format(ERROR_CATEGORY_DOES_NOT_EXIST, model["category"].asString()));

        model["name"] = options.at("name");
        model["price"] = std::stod(options.at("price"));

        model["location"] = options.contains("location") ? options.at("location") : ConfigManager::GetInstance().GetDefaultLocation();

        if (flags.contains("yesterday"))
            model["date"] = db::util::GetYesterdayDate();
        else
            model["date"] = options.contains("date") ? options.at("date") : db::util::GetCurrentDate();

        if (options.contains("tags"))
        {
            std::string tag;
            if(!GenerateTags(options.at("tags"), tag))
                return Result::Create(StatusCode::DBError, std::format(ERROR_TAG_DOES_NOT_EXIST, options.at("tags")));

            model["tags"] = tag;
        }

        if (!expenseTable->Insert(model))
        {
            ERROR_LOG(ERROR_DB_INSERT_EXPENSE, model["name"].asString());
            return Result::Create(StatusCode::DBError, std::format(ERROR_DB_INSERT_EXPENSE, model["name"].asString()));
        }

        return Result::Success();
    }

    ResultSPtr Add::ValidateMandatoryArguments(
        const std::unordered_set<std::string>& flags,
        const std::map<std::string, std::string>& options) const
    {
        // name, category, price must be present
        if (!options.contains("name"))
            return Result::Create(StatusCode::MandatoryArgNotPresent, std::format(ERROR_MANDATORY_ARG_NOT_PRESENT, "name"));

        if (!options.contains("category"))
            return Result::Create(StatusCode::MandatoryArgNotPresent, std::format(ERROR_MANDATORY_ARG_NOT_PRESENT, "category"));

        if (!options.contains("price"))
            return Result::Create(StatusCode::MandatoryArgNotPresent, std::format(ERROR_MANDATORY_ARG_NOT_PRESENT, "price"));

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