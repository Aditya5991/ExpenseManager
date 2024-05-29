#include "pch.h"
#include "Account/Manager.h"
#include "ReportHandler.h"
#include "DBHandler/Util.h"
#include "DatabaseManager.h"
#include "DBTable_Category.h"
#include "DBTable_Expense.h"
#include "Conditions.h"
#include "TextTable.h"

namespace em
{

    ReportHandler::ReportHandler()
    {
    }

    //public
    StatusCode ReportHandler::GenerateReport(Option option, const std::string& month, const std::string& year)
    {
        m_Unit = std::stoi(month);

        db::Condition cond;
        GenerateCondition(cond, option, month, year);

        std::vector<DBModel_Category> categories;
        auto categoryTable = databaseMgr.GetTable<DBTable_Category>();
        categoryTable->Select(categories);

        // for each expense table, go through each category, and store the expenses.
        const Map_DBTables& expenseTables = em::DatabaseManager::GetInstance().GetExpenseTables();
        for (const auto& iter : expenseTables)
        {
            std::shared_ptr<DBTable_Expense> expenseTable = iter.second;

            for (const DBModel_Category& category : categories)
            {
                cond.Add(Condition_Category::Create(category.Name));

                std::vector<DBModel_Expense> expenses;
                expenseTable->Select(expenses, cond);

                double total = 0.0;
                for (const DBModel_Expense& expense : expenses)
                    total += expense.Price;

                if (total != 0.0)
                    m_Prices[category.Name] += total;

                // need to pop the last condition so that category can be changed
                cond.PopBack();
            }
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
            cond.Add(Condition_Date::Create(db::util::GetCurrentDate()));
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