#include "EM/pch.h"
#include "Report.h"
#include "EM/DatabaseManager.h"
#include "EM/Conditions.h"
#include "EM/Renderer_TextTable.h"

#include "DBHandler/Util.h"
#include "DBHandler/Table.h"

namespace em::action_handler::cli
{

    em::action_handler::ResultSPtr Report::Execute(
        const std::string& commandName,
        const std::unordered_set<std::string>& flags,
        const std::map<std::string, std::string>& options)
    {
        assert(commandName == "report");
        ReportHandler report;

        if (flags.contains("today"))
        {
            report.GenerateReport(ReportHandler::TODAY);
            report.Print();
            return Result::Success();
        }

        std::string month = "-1";
        std::string year = "-1";
        ReportHandler::Option option = ReportHandler::ALL;

        bool hasParamMonth = options.contains("month");
        bool hasParamYear = options.contains("year");

        // check if month and year both are specified
        if (hasParamMonth && hasParamYear)
        {
            month = options.at("month");
            year = options.at("year");
            option = ReportHandler::MONTH_AND_YEAR;
        }
        // check if only month is specified.
        else if (hasParamMonth)
        {
            month = options.at("month");
            option = ReportHandler::MONTH;
        }
        // check if only year is specified.
        else if (hasParamYear)
        {
            year = options.at("year");
            option = ReportHandler::YEAR;
        }
        // check if --thisMonth is specified
        else if (flags.contains("thisMonth"))
        {
            option = ReportHandler::MONTH;
            month = db::DateTime::GetThisMonth();
        }
        // check if --thisYear is specified
        else if (flags.contains("thisYear"))
        {
            option = ReportHandler::YEAR;
            year = db::DateTime::GetThisYear();
        }

        em::utils::date::FixMonthName(month);
        report.GenerateReport(option, month, year);
        report.Print();

        return Result::Success();
    }

}