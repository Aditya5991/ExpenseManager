#include "EM/pch.h"
#include "List.h"
#include "EM/DatabaseManager.h"
#include "EM/Conditions.h"
#include "EM/Renderer_TextTable.h"
#include "EM/Account/Account.h"
#include "EM/Account/Manager.h"

#include "DBHandler/Util.h"
#include "DBHandler/Table.h"
#include "Utilities/StringUtils.h"
#include "DBHandler/DateTime.h"

namespace em::action_handler::cli
{

    // protected
    em::action_handler::ResultSPtr List::Execute(
        const std::string& commandName,
        const std::unordered_set<std::string>& flags,
        const std::map<std::string, std::vector<std::string>>& options)
    {
        assert(commandName == "list");

        if (flags.contains("categories"))
            return ListCategories();

        if (flags.contains("tags"))
            return ListTags();

        if (flags.contains("accounts"))
            return ListAccounts();

        db::Condition finalCondition;
        AppendAccountCondition(finalCondition);

        // handle dates
        if (flags.contains("thisMonth"))
            finalCondition.Add(Condition_Month::Create(db::DateTime::GetThisMonth()));
        if (flags.contains("today"))
            finalCondition.Add(Condition_Date::Create(db::DateTime::GetCurrentDate().AsString()));
        if (flags.contains("thisYear"))
            finalCondition.Add(Condition_Year::Create(db::DateTime::GetThisYear()));

        if (flags.contains("yesterday"))
        {
            const std::string& date = db::DateTime::GetYesterdayDate().AsString();
            finalCondition.Add(Condition_Date::Create(date));
        }
        else if (options.contains("date"))
        {
            const std::string& date = options.at("date").front();
            finalCondition.Add(Condition_Date::Create(date));
        }

        if (options.contains("month") && options.contains("year"))
        {
            std::string month = options.at("month").front();
            utils::date::FixMonthName(month);

            const std::string& year = options.at("year").front();
            finalCondition.Add(Condition_Month::Create(month, year));
        }
        else if (options.contains("month"))
        {
            std::string month = options.at("month").front();
            utils::date::FixMonthName(month);
            finalCondition.Add(Condition_Month::Create(month));
        }
        else if (options.contains("year"))
        {
            const std::string& year = options.at("year").front();
            finalCondition.Add(Condition_Year::Create(year));
        }

        // handle name
        if (options.contains("name"))
        {
            std::string name = options.at("name").front();
            finalCondition.Add(Condition_ListNameFilter::Create(name));
        }

        // handle location
        if (options.contains("location"))
        {
            std::string location = options.at("location").front();
            finalCondition.Add(Condition_LocationFilter::Create(location));
        }

        // handle categories
        if (options.contains("category"))
        {
            auto result = AppendCategoryCondition(finalCondition, options.at("category").front());
            if (result->statusCode != StatusCode::Success)
                return result;
        }
        else if (options.contains("ignoreCategory"))
        {
            std::vector<std::string> categories;
            em::utils::string::SplitString(options.at("ignoreCategory").front(), categories);
            for (const std::string& category : categories)
            {
                auto categoriesTable = databaseMgr.GetTable("categories");
                db::Model categoryModel;
                if (!categoriesTable->CheckIfExists("name", category, &categoryModel))
                    continue;

                finalCondition.Add(Condition_IgnoreCategory::Create(categoryModel["row_id"]));
            }
        }

        bool showTags = flags.contains("showTags");
        if (options.contains("tags"))
        {
            auto result = AppendTagsCondition(finalCondition, options.at("tags").front());
            if (result->statusCode != StatusCode::Success)
                return result;

            showTags = true;
        }
        else if (options.contains("ignoreTags"))
        {
            const std::string& commaSeparatedTagsToIgnore = options.at("ignoreTags").front();
            db::Condition* ignoreTagsCond = CreateIgnoreTagsCondition(commaSeparatedTagsToIgnore);
            finalCondition.Add(ignoreTagsCond);
        }
        else if (flags.contains("excludeTags"))
        {
            finalCondition.Add(Condition_ListTagFilter::Create(""));
        }

        bool showAccount = flags.contains("showAccount");
        bool showLocation = flags.contains("showLocation");

        // handle the order by clause
        const std::string orderByKey = options.contains("orderBy") ? options.at("orderBy").front() : "";
        bool isDecending = flags.contains("descending");
        db::Clause_OrderBy orderBy = std::move(GetOrderByClause(orderByKey, isDecending));

        if (options.contains("range"))
        {
            db::DateTime startDate = options.at("range").front();
            db::DateTime endDate = options.at("range").back();

            if (endDate < startDate)
                return Result::GeneralFailure("StartDate cannot be greater than EndDate!");

            return ProcessDBTableWithDateRange(finalCondition, orderBy, startDate, endDate, showTags, showAccount, showLocation);
        }

        if (flags.contains("byDate"))
            return DisplayByDate(finalCondition, orderBy);

        return ProcessDBTable(finalCondition, orderBy, showTags, showAccount, showLocation);
    }

    // protected
    em::action_handler::ResultSPtr List::ProcessDBTable(
        const db::Condition& dbCondition,
        const db::Clause_OrderBy& orderBy,
        bool showTags,
        bool showAccount,
        bool showLocation)
    {
        std::vector<db::Model> rows;

        auto expenseTable = databaseMgr.GetTable("expenses");
        if (!expenseTable->Select(rows, dbCondition, orderBy))
            return Result::Create(StatusCode::DBError, "Failed to retrieve from table!");

        const std::string& currentAccountName = em::account::Manager::GetInstance().GetCurrentAccountName();

        double totalExpense = expenseTable->SumOf("price", dbCondition);
        Renderer_ExpenseTable::Render(currentAccountName, rows, totalExpense, showTags, showAccount, showLocation);

        return Result::Success();
    }

    // protected
    db::Clause_OrderBy List::GetOrderByClause(const std::string& orderByKey, bool isDescending) const
    {
        db::Clause_OrderBy::OrderType orderType = isDescending ? db::Clause_OrderBy::DESCENDING : db::Clause_OrderBy::ASCENDING;

        if (!orderByKey.empty())
            return db::Clause_OrderBy(orderByKey, orderType);

        return db::Clause_OrderBy("date", orderType);
    }

    // protected
    em::action_handler::ResultSPtr List::ListCategories()
    {
        std::vector<db::Model> rows;

        auto table = databaseMgr.GetTable("categories");
        if (!table->Select(rows))
        {
            ERROR_LOG(ERROR_DB_SELECT_CATEGORY);
            return em::action_handler::Result::Create(StatusCode::DBError, ERROR_DB_SELECT_CATEGORY);
        }

        Renderer_CategoryTable::Render(rows);
        return em::action_handler::Result::Success();
    }

    // protected
    em::action_handler::ResultSPtr List::ListTags()
    {
        std::vector<db::Model> rows;

        auto table = databaseMgr.GetTable("tags");
        if (!table->Select(rows))
        {
            ERROR_LOG(ERROR_DB_SELECT_TAG);
            return em::action_handler::Result::Create(StatusCode::DBError, ERROR_DB_SELECT_TAG);
        }

        Renderer_CategoryTable::Render(rows);
        return em::action_handler::Result::Success();
    }

    // protected
    em::action_handler::ResultSPtr List::ListAccounts()
    {
        std::vector<db::Model> rows;

        auto table = databaseMgr.GetTable("accounts");
        if (!table->Select(rows))
        {
            ERROR_LOG(ERROR_DB_SELECT_TAG);
            return em::action_handler::Result::Create(StatusCode::DBError, ERROR_DB_SELECT_TAG);
        }

        Renderer_AccountTable::Render(rows);
        return em::action_handler::Result::Success();
    }

    // private
    em::action_handler::ResultSPtr List::AppendCategoryCondition(
        db::Condition& finalCondition,
        const std::string& commaSeparatedCategtories) const
    {
        db::Condition* categoryConditions = new db::Condition(db::Condition::RelationshipType::OR);
        std::vector<std::string> categories;
        em::utils::string::SplitString(commaSeparatedCategtories, categories);

        auto table = databaseMgr.GetTable("categories");
        for (const std::string& category : categories)
        {
            // check if the category is valid.
            db::Model categoryModel;
            if (!table->CheckIfExists("name", category, &categoryModel))
            {
                return em::action_handler::Result::Create(
                    StatusCode::CategoryDoesNotExist,
                    std::format(ERROR_CATEGORY_DOES_NOT_EXIST, category));
            }

            categoryConditions->Add(Condition_Category::Create(categoryModel["row_id"]));
        }

        finalCondition.Add(categoryConditions);

        return em::action_handler::Result::Success();
    }

    // private
    em::action_handler::ResultSPtr List::AppendAccountCondition(db::Condition& finalCondition) const
    {
        int accountId = em::account::Manager::GetInstance().GetCurrentAccountId();
        std::string accountIdAsStr = std::to_string(accountId);
        finalCondition.Add(new db::Condition("account_id", accountIdAsStr, db::Condition::Type::EQUALS));
        return em::action_handler::Result::Success();
    }

    // private
    em::action_handler::ResultSPtr List::AppendTagsCondition(db::Condition& finalCondition, const std::string& commaSeparatedTags) const
    {
        db::Condition* tagConditions = new db::Condition(db::Condition::RelationshipType::OR);
        std::vector<std::string> tags = ::utils::string::SplitString(commaSeparatedTags, ',');

        auto table = databaseMgr.GetTable("tags");
        for (const std::string& tag : tags)
        {
            db::Condition* tagExistsCondition = Condition_Tag::Create(tag);
            // check if the tag is valid.
            if (!table->CheckIfExists(*tagExistsCondition))
            {
                delete tagExistsCondition;
                return em::action_handler::Result::Create(
                    StatusCode::CategoryDoesNotExist,
                    std::format(ERROR_TAG_DOES_NOT_EXIST, tag));
            }
            delete tagExistsCondition;

            tagConditions->Add(Condition_ListTagFilter::Create(tag));
        }

        finalCondition.Add(tagConditions);

        return em::action_handler::Result::Success();
    }

    // protected
    db::Condition* List::CreateIgnoreTagsCondition(const std::string& commaSeparatedTagsToIgnore)
    {
        std::vector<std::string> tags = utils::string::SplitString(commaSeparatedTagsToIgnore);

        // no need to check if a tag exists in the database
        db::Condition* finalCondition = new db::Condition(db::Condition::RelationshipType::AND);
        for (const std::string& tag : tags)
            finalCondition->Add(Condition_IgnoreTags::Create(tag));

        return finalCondition;
    }

    // protected
    em::action_handler::ResultSPtr List::DisplayByDate(const db::Condition& cond, const db::Clause_OrderBy& orderBy) const
    {
        try 
        {
            std::vector<db::Model> rows;
            GetExpenses(rows, cond, orderBy);

            std::map<db::DateTime, double> pricesByDate;
            for (const db::Model& row : rows)
            {
                db::DateTime date(row.at("date").asString());
                double price = row.at("price").asDouble();
                pricesByDate[date] += price;
            }

            Renderer_ExpenseTable_ByDate::Render(pricesByDate);
        }
        catch (std::exception& ex)
        {
            printf("\nEXCEPTION: %s", ex.what());
            return Result::GeneralFailure(std::string(ex.what()));
        }

        return Result::Success();
    }

    // protected
    em::action_handler::ResultSPtr List::ProcessDBTableWithDateRange(
        const db::Condition& cond,
        const db::Clause_OrderBy& orderBy,
        const db::DateTime& startDate,
        const db::DateTime& endDate,
        bool showTags,
        bool showAccount,
        bool showLocation)
    {
        try
        {
            std::vector<db::Model> rows;

            auto expenseTable = databaseMgr.GetTable("expenses");
            if (!expenseTable->Select(rows, cond, orderBy))
                return Result::Create(StatusCode::DBError, "Failed to retrieve from table!");

            std::vector<db::Model> rowsWithinRange;
            std::for_each(rows.begin(), rows.end(), 
                [&](const db::Model& row) 
                {
                    const db::DateTime& dateTime = row.at("date").asDateTime();
                    if (startDate <= dateTime && dateTime <= endDate)
                        rowsWithinRange.push_back(row);
                });

            // sort according to price, highest to lowest
            std::sort(rowsWithinRange.begin(), rowsWithinRange.end(),
                [](db::Model& e1, db::Model& e2)
                {
                    return e1["date"].asDateTime() < e2["date"].asDateTime();
                });

            const std::string& currentAccountName = em::account::Manager::GetInstance().GetCurrentAccountName();

            double totalExpense = 0.0;
            std::for_each(rowsWithinRange.begin(), rowsWithinRange.end(), 
                [&totalExpense](const db::Model& row) 
                {
                    totalExpense += row.at("price").asDouble(); 
                });

            Renderer_ExpenseTable::Render(currentAccountName, rowsWithinRange, totalExpense, showTags, showAccount, showLocation);
            return Result::Success();
        }
        catch (std::exception& ex)
        {
            printf("\nEXCEPTION: %s", ex.what());
            return Result::GeneralFailure(ex.what());
        }
    }

    // protected
    void List::GetExpenses(std::vector<db::Model>& rows, const db::Condition& cond, const db::Clause_OrderBy& orderBy) const
    {
        auto expenseTable = databaseMgr.GetTable("expenses");
        if (!expenseTable->Select(rows, cond, orderBy))
            printf("\nFailed to fetch rows!");
    }

}