#pragma once

#include "DBHandler/Table.h"
#include "DBHandler/DateTime.h"
#include "TextTable.h"
#include "ReportHandler.h"
#include <unordered_map>

namespace em
{
    class Renderer_ExpenseTable_ByDate
    {
    public:

        static void Render(const std::map<db::DateTime, double>& pricesByDate)
        {
            printf("\n Total Rows : %zd", pricesByDate.size());

            TextTable_Expense_ByDate t(pricesByDate);
            t.Print();
        }
    };

    /**
    * Helper class that can be used to display data from DBTable_Expense in the form of table for CLI.
    */
    class Renderer_ExpenseTable
    {
    public:

        /**
        * This function renders the data in table format.
        *
        * @params [in] rows
        *       Rows representing the rows from DBTable_Expense that needs to be displayed in the table.
        *
        * @params [in] total
        *       This arg is used to display the 'price' parameter's total.
        */
        static void Render(
            const std::string& accountName, 
            const std::vector<db::Model>& rows, 
            double total = 0.0, 
            bool showTags = false, 
            bool showAccount = false,
            bool showLocation = false)
        {
            printf("\n Total Rows : %zd", rows.size());
            TextTable_Expense t(accountName, rows, showTags, showAccount, showLocation);
            t.Print();

            if (total != 0.0)
            {
                TextTable_TotalExpense tx(total);
                tx.Print();
            }
        }

        /**
        * This function renders the data in table format.
        *
        * @params [in] rows
        *       A map with key as the accountName that the expenses belong to, and value as the actual expense records for the given account.
        *
        * @params [in] total
        *       This arg is used to display the 'price' parameter's total.
        */
        static void Render(const std::unordered_map<std::string, std::vector<db::Model>>& rows, double total = 0.0)
        {
            size_t totalRows = 0;
            std::for_each(rows.begin(), rows.end(),
                [&](const auto& iter)
                {
                    totalRows += iter.second.size();
                });

            printf("\n Total Rows : %zd", totalRows);
            TextTable_Expense t(rows);
            t.Print();

            if (total != 0.0)
            {
                TextTable_TotalExpense tx(total);
                tx.Print();
            }
        }

    };

    /**
    * Helper class that can be used to display data from DBTable_Category in the form of table for CLI.
    */
    class Renderer_CategoryTable
    {
    public:

        /**
        * This function renders the data in table format.
        *
        * @params [in] rows
        *       Rows representing the rows from DBTable_Category that needs to be displayed in the table.
        */
        static void Render(const std::vector<db::Model>& rows)
        {
            printf("\n Total Rows : %zd", rows.size());
            TextTable_Category t(rows);
            t.Print();
        }

    };

    class Renderer_CompareReport
    {
    public:

        static void Render(const std::vector<ReportHandler>& reports)
        {
            TextTable_CompareReport t(reports);
            t.Print();
        }

    };

}