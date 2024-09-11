#include "pch.h"
#include "Account/Manager.h"
#include "ReportHandler.h"
#include "DatabaseManager.h"
#include "Conditions.h"
#include "TextTable.h"

#include "DBHandler/Util.h"
#include "DBHandler/Table.h"

namespace em
{

    ReportHandler::ReportHandler()
    {
    }

    //public
    StatusCode ReportHandler::GenerateReport(Option option, const std::string& month, const std::string& year, bool includeZeroExpense)
    {
        m_Unit = std::stoi(month);

        db::Condition cond;
        GenerateCondition(cond, option, month, year);

        std::vector<db::Model> categories;
        auto categoryTable = databaseMgr.GetTable("categories");
        categoryTable->Select(categories);

        // for each expense table, go through each category, and store the expenses.
        const std::string& expenseTableName = databaseMgr.GetCurrentExpenseTableName();
        auto expenseTable = databaseMgr.GetTable(expenseTableName);

        for (const db::Model& category : categories)
        {
            cond.Add(Condition_Category::Create(category.at("row_id").asInt()));

            std::vector<db::Model> expenses;
            expenseTable->Select(expenses, cond);

            double total = 0.0;
            for (const db::Model& expense : expenses)
                total += expense.at("price").asDouble();

            if(includeZeroExpense || total != 0.0)
                m_Prices[category.at("name").asString()] += total;

            // need to pop the last condition so that category can be changed
            cond.PopBack();
        }

        return StatusCode::Success;
    }

    //public
    void ReportHandler::Print(bool sort)
    {
        std::vector<std::pair<std::string, double>> pricesAsVector;
        em::utils::ConvertToVector(m_Prices, pricesAsVector, sort);

        double totalExpense = 0.0;

        TextTable t;
        t.add("Category").add("Total").endOfRow();
        for (const auto& [categoryName, price] : pricesAsVector)
        {
            t.add(categoryName)
                .add(std::to_string(price))
                .endOfRow();

            totalExpense += price;
        }

        printf("\n%s", t.toString().c_str());

        TextTable expenseTable;
        expenseTable.add("TOTAL").add(std::to_string(totalExpense)).endOfRow();
        printf("\n%s", expenseTable.toString().c_str());
    }

    // private
    void ReportHandler::GenerateCondition(db::Condition& cond, Option option, const std::string& month, const std::string& year) const
    {
        switch (option)
        {
        case Option::TODAY:
            cond.Add(Condition_Date::Create(db::DateTime::GetCurrentDate().AsString()));
            break;
        case Option::MONTH:
            cond.Add(Condition_Month::Create(month));
            break;
        case Option::YEAR:
            cond.Add(Condition_Year::Create(year));
            break;
        case Option::MONTH_AND_YEAR:
            cond.Add(Condition_MonthAndYear::Create(month, year));
            break;
        }
    }
}