#include "EM/pch.h"
#include "AddReminder.h"
#include "EM/DatabaseManager.h"
#include "DBHandler/DateTime.h"

#include "DBHandler/Table.h"
#include "Utilities/StringUtils.h"
#include "Utilities/GeneralUtils.h"

namespace em::action_handler::cli
{

    ResultSPtr AddReminder::Execute(
        const std::string& commandName,
        const std::unordered_set<std::string>& flags,
        const std::map<std::string, std::vector<std::string>>& options)
    {
        assert(commandName == "addReminder");

        ReminderType reminderType = GetTypeAsEnum(options.at("type").front());
        if (reminderType == ReminderType::Invalid || !Validate(reminderType, options.at("dateData").front()))
            return Result::Create(StatusCode::GeneralFailure, "AddReminder: Input Validation failed!");

        db::Model model;
        model["name"] = options.at("name").front();
        model["type"] = static_cast<int>(reminderType);
        model["date_data"] = options.at("dateData").front();

        auto table = databaseMgr.GetTable("reminders");
        if (!table->Insert(model))
        {
            ERROR_LOG(ERROR_DB_INSERT_REMINDER, options.at("name").front());
            return Result::Create(StatusCode::DBError, std::format(ERROR_DB_INSERT_REMINDER, options.at("name").front()));
        }

        return Result::Success();
    }

    // private
    bool AddReminder::Validate(ReminderType type, const std::string& dateData) const
    {
        if (type == ReminderType::OneTime)
        {
            if (!db::DateTime::IsValidDateString(dateData))
            {
                printf("\nInvalid Date: %s", dateData.c_str());
                return false;
            }

            db::DateTime todaysDate = db::DateTime::GetCurrentDate();
            db::DateTime inputDate(dateData);

            if (inputDate < todaysDate)
                return false;
        }
        else if (type == ReminderType::Weekly)
        {
            if (!db::DateTime::IsValidDayName(dateData))
            {
                printf("\nInvalid Day Name: %s", dateData.c_str());
                return false;
            }
        }
        else if (type == ReminderType::Monthly)
        {
            if (!utils::general::IsNumeric<int>(dateData))
            {
                printf("\nInvalid Month Name, must be string: %s", dateData.c_str());
                return false;
            }

            int day = std::stoi(dateData);
            if (!db::DateTime::IsValidDay(day))
            {
                printf("\nInvalid Day, must be in bounds: %s", dateData.c_str());
                return false;
            }
        }
        else if (type == ReminderType::Yearly)
        {
            std::vector<std::string> dayMonth = utils::string::SplitString(dateData, '-');
            if (dayMonth.size() != 2)
            {
                printf("\nInvalid Day-Month Format: %s", dateData.c_str());
                return false;
            }

            std::string month = dayMonth.front();
            std::string day = dayMonth.back();
            if (!utils::general::IsInteger(day) || !utils::general::IsInteger(month))
            {
                printf("\nInvalid Day Month, both must be integers: %s", dateData.c_str());
                return false;
            }

            if (!db::DateTime::IsValidDay(std::stoi(day)) || !db::DateTime::IsValidMonth(std::stoi(month)))
            {
                printf("\nInvalid Day Month, both must be in bounds: %s", dateData.c_str());
                return false;
            }
        }

        return true;
    }

    ReminderType AddReminder::GetTypeAsEnum(const std::string& enumAsStr) const
    {
        if (enumAsStr == "OneTime")
            return ReminderType::OneTime;

        if (enumAsStr == "Daily")
            return ReminderType::Daily;

        if (enumAsStr == "Weekly")
            return ReminderType::Weekly;

        if (enumAsStr == "Monthly")
            return ReminderType::Monthly;

        if (enumAsStr == "Yearly")
            return ReminderType::Yearly;

        return ReminderType::Invalid;
    }

}