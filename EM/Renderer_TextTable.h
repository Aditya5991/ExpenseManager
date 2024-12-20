#pragma once

#include "DBHandler/Table.h"
#include "DBHandler/DateTime.h"
#include "TextTable.h"
#include "ReportHandler.h"
#include <unordered_map>
#include "Utilities/StringUtils.h"

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

    class Renderer_CompareReport
    {
    public:

        static void Render(const std::vector<ReportHandler>& reports)
        {
            TextTable_CompareReport t(reports);
            t.Print();
        }

    };

    /**
    * Helper class that can be used to display Account data in the form of table for CLI.
    */
    class Renderer_AccountTable
    {
    public:

        /**
        * This function renders the data in table format.
        *
        * @params [in] rows
        *       Rows representing the rows from db::Model of Account that needs to be displayed in the table.
        */
        static void Render(const std::vector<db::Model>& rows)
        {
            printf("\n Total Rows : %zd", rows.size());
            TextTable_Account t(rows);
            t.Print();
        }

    };

    class Renderer_Generic
    {
    public:
        static void Render(const std::vector<db::Model>& rows)
        {
            printf("\n Total Rows : %zd", rows.size());
            TextTable t;

            for (auto iter : rows[0])
                t.add(::utils::string::ToUpper(iter.first));
            t.endOfRow();

            for (size_t i = 0; i < rows.size(); ++i)
            {
                for (auto iter : rows[i])
                {
                    const db::DBValue& value = iter.second;
                    if (value.IsInt())
                    {
                        t.add(std::to_string(value.asInt()));
                    }
                    else if (value.IsDouble())
                    {
                        t.add(std::to_string(value.asDouble()));
                    }
                    else if (value.IsString())
                    {
                        t.add(value.asString());
                    }
                    else if (value.IsBool())
                    {
                        t.add(value.asBool() ? "TRUE" : "FALSE");
                    }
                }
                t.endOfRow();
            }

            t.Print();
        }
    };

}