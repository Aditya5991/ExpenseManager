#include "EM/pch.h"
#include "Update.h"
#include "EM/DatabaseManager.h"
#include "EM/Conditions.h"
#include "EM/ConfigManager.h"
#include "EM/Exceptions/General.h"

#include "DBHandler/Util.h"
#include "DBHandler/Table.h"
#include "Utilities/StringUtils.h"
#include "Utilities/GeneralUtils.h"

namespace em::action_handler::cli
{

    ResultSPtr Update::Execute(
        const std::string& commandName,
        const std::unordered_set<std::string>& flags,
        const std::map<std::string, std::string>& options)
    {
        assert(commandName == "update");

        int rowId = std::atoi(options.at("row_id").c_str());

        auto expenseTable = databaseMgr.GetTable(databaseMgr.GetCurrentExpenseTableName());
        db::Model origModel;
        if (!expenseTable->SelectById(origModel, rowId))
            return Result::GeneralFailure(std::format("ERROR: Entity with rows_id - % d does not exist!", rowId));

        std::string attributeName = options.at("attributeName");
        std::string attributeValue = options.at("attributeValue");

        ResultSPtr validationResult = Validate(attributeName, attributeValue);
        if (validationResult->statusCode != StatusCode::Success)
            return validationResult;

        // TODO: handle foreign keys differently, not like this.
        if (expenseTable->IsForeignKeyAccessName(attributeName))
            HandleForeignKeyUpdate(attributeName, attributeValue);

        db::Model newModel = origModel;
        newModel[attributeName] = attributeValue;

        if (!expenseTable->Update(origModel, newModel))
        {
            ERROR_LOG(ERROR_DB_UPDATE_EXPENSE, newModel["name"].asString());
            return Result::Create(StatusCode::DBError, std::format(ERROR_DB_UPDATE_EXPENSE, newModel["name"].asString()));
        }

        return Result::Success();
    }

    void Update::HandleForeignKeyUpdate(std::string& attributeName, std::string& attributeValue)
    {
        if (attributeName == "category")
        {
            // get the category_id and update it
            db::Model categoryModel;
            if (databaseMgr.GetTable("categories")->Select(categoryModel, *Condition_CategoryName::Create(attributeValue)))
            {
                attributeName = "category_id";
                attributeValue = std::to_string(categoryModel["row_id"].asInt());
            }
        }
    }

    ResultSPtr Update::Validate(const std::string& attributeName, const std::string& attributeValue)
    {
        auto expenseTable = databaseMgr.GetTable(databaseMgr.GetCurrentExpenseTableName());
        if (!expenseTable->IsForeignKeyAccessName(attributeName) && !expenseTable->IsValidColumnName(attributeName))
            return Result::GeneralFailure(std::format("Invalid attributeName: '{}'", attributeName));

        // check Category, must exist in the database
        if (attributeName == "category")
        {
            auto categoryTable = databaseMgr.GetTable("categories");
            if (!categoryTable->CheckIfExists("name", attributeValue))
            {
                ERROR_LOG(ERROR_CATEGORY_DOES_NOT_EXIST, attributeValue);
                return Result::Create(StatusCode::DBError, std::format(ERROR_CATEGORY_DOES_NOT_EXIST, attributeValue));
            }
        }

        // check price, must be a double
        if (attributeName == "price")
        {
            if (!utils::general::IsDouble(attributeValue))
            {
                ERROR_LOG(ERROR_INVALID_ATTRIBUTE_TYPE, attributeValue);
                return Result::Create(StatusCode::Invalid, std::format(ERROR_INVALID_ATTRIBUTE_TYPE, attributeValue));
            }
        }

        // check tags, must exist in the database
        if (attributeName == "tags")
        {
            auto tagsTable = databaseMgr.GetTable("tags");
            std::vector<std::string> tags = utils::string::SplitString(attributeValue, ',');
            std::unordered_set<std::string> uniqueTags;
            for (const std::string& tag : tags)
            {
                if (!tagsTable->CheckIfExists("name", tag))
                {
                    ERROR_LOG(ERROR_TAG_DOES_NOT_EXIST, tag);
                    return Result::Create(StatusCode::DBError, std::format(ERROR_TAG_DOES_NOT_EXIST, tag));
                }
            }
        }

        return Result::Success();
    }

}