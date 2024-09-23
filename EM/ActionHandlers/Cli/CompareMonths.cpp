#include "../../pch.h"
#include "CompareMonths.h"
#include "../../ReportHandler.h"
#include "../../Renderer_TextTable.h"
#include "DBHandler/Util.h"
#include "EM/DatabaseManager.h"
#include "EM/Conditions.h"
#include "EM/ReportHandler.h"

#include <numeric>

namespace em::action_handler::cli
{
    em::action_handler::ResultSPtr CompareMonths::Execute(
        const std::string& commandName,
        const std::unordered_set<std::string>& flags,
        const std::map<std::string, std::vector<std::string>>& options)
    {
        assert(commandName == "compareMonths");

        int startMonth = 1;
        int endMonth = 12;
        std::string year = db::DateTime::GetThisYear();
        if (options.contains("range"))
        {
            bool isValid = ValidateRangeParameter(options.at("range"), startMonth, endMonth);
            if (!isValid)
                return Result::Create(StatusCode::InvalidParameterValue, "Wrong parameters for range!");
        }

        if (options.contains("year"))
        {
            year = options.at("year").front();
            if (!em::utils::date::IsValidYear(year))
                return Result::Create(StatusCode::InvalidParameterValue, std::format("Invalid year: {}", year));
        }

        return CompareAllMonths(year, startMonth, endMonth);
    }

    em::action_handler::ResultSPtr CompareMonths::CompareAllMonths(const std::string& year, int startMonth, int endMonth)
    {
        std::vector<ReportHandler> reports;
        for (int month = startMonth; month <= endMonth; ++month)
        {
            std::string monthAsString = std::to_string(month);
            em::utils::date::FixMonthName(monthAsString);

            ReportHandler report;
            report.GenerateReport(ReportHandler::MONTH_AND_YEAR, monthAsString, year, true);
            if(report.GetPrices().size() > 0)
                reports.push_back(std::move(report));
        }

        Renderer_CompareReport::Render(reports);

        return Result::Success();
    }

    bool CompareMonths::ValidateRangeParameter(const std::vector<std::string>& rangeValueStr, int& startMonth, int& endMonth)
    {
        if (rangeValueStr.size() != 2)
        {
            printf("-range parameter must have value as 'startMonth endMonth'");
            return false;
        }

        std::string startMonthStr = rangeValueStr.front();
        std::string endMonthStr = rangeValueStr.back();

        if (!em::utils::IsInteger(startMonthStr) || !em::utils::IsInteger(endMonthStr))
        {
            printf("\n-range parameter must have integers");
            return false;
        }

        startMonth = std::stoi(startMonthStr);
        endMonth = std::stoi(endMonthStr);

        if (startMonth < 1 || startMonth > 12)
        {
            printf("\nstartMonth must be between 1 to 12");
            return false;
        }

        if (endMonth < 1 || endMonth > 12)
        {
            printf("\nendMonth must be between 1 to 12");
            return false;
        }

        if (startMonth >= endMonth)
        {
            printf("\nstartMonth should be less than endMonth");
            return false;
        }

        return true;
    }

}