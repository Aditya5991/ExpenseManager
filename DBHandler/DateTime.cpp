#include "Common.h"
#include "DateTime.h"
#include <istream>


BEGIN_NAMESPACE_DB

DateTime::DateTime(tm t, char separator)
    :m_Second(t.tm_sec)
    , m_Minute(t.tm_min)
    , m_Hour(t.tm_hour)
    , m_Day(t.tm_mday)
    , m_Month(t.tm_mon + 1)
    , m_Year(t.tm_year + 1900)
    , m_Separator(separator)
{
}

DateTime::DateTime(int year, int month, int day, char separator)
    : m_Year(year)
    , m_Month(month)
    , m_Day(day)
    , m_Hour(0)
    , m_Minute(0)
    , m_Second(0)
    , m_Separator(separator)
{
}

DateTime::DateTime(const std::string& dateStr, char separator)
    : m_Separator(separator)
{
    try
    {
        std::stringstream ss(dateStr);
        char dash; // to read the separator character
        ss >> m_Year >> dash >> m_Month >> dash >> m_Day;
    }
    catch (std::exception& ex)
    {
        printf("\nEXCEPTION: Failed to parse Date - %s", dateStr.c_str());
        throw ex;
    }
}

bool DateTime::operator () (const DateTime& d1, const DateTime& d2) const
{
    return d1 < d2;
}

bool DateTime::operator < (const DateTime& other) const
{
    if (m_Year != other.m_Year)
        return m_Year < other.m_Year;

    if (m_Month != other.m_Month)
        return m_Month < other.m_Month;

    return m_Day < other.m_Day;
}

bool DateTime::operator > (const DateTime& other) const
{
    if (m_Year != other.m_Year)
        return m_Year > other.m_Year;

    if (m_Month != other.m_Month)
        return m_Month > other.m_Month;

    return m_Day > other.m_Day;
}

bool DateTime::operator <= (const DateTime& other) const
{
    if (m_Year != other.m_Year)
        return m_Year <= other.m_Year;

    if (m_Month != other.m_Month)
        return m_Month <= other.m_Month;

    return m_Day <= other.m_Day;
}

bool DateTime::operator >= (const DateTime& other) const
{
    if (m_Year != other.m_Year)
        return m_Year >= other.m_Year;

    if (m_Month != other.m_Month)
        return m_Month >= other.m_Month;

    return m_Day >= other.m_Day;
}


bool DateTime::operator==(const DateTime& other) const
{
    return (other.GetYear() == GetYear() &&
            other.GetMonth() == GetMonth() &&
            other.GetDay() == GetDay() &&
            other.GetHour() == GetHour() &&
            other.GetMinute() == GetMinute() &&
            other.GetSecond() == GetSecond());
}

std::string DateTime::AsString(bool useTime) const
{
    if(!useTime)
        return std::format("{}{}{}{}{}", 
            (m_Year < 10 ? "0" + std::to_string(m_Year) : std::to_string(m_Year)),
            m_Separator,
            (m_Month < 10 ? "0" + std::to_string(m_Month) : std::to_string(m_Month)),
            m_Separator, 
            (m_Day < 10 ? "0" + std::to_string(m_Day) : std::to_string(m_Day)));

    return "";
}

DateTime DateTime::GetYesterday()
{
    std::time_t now = std::time(0);
    std::time_t yesterday = now - (static_cast<long long>(60 * 60) * 24);
    tm t;
    localtime_s(&t, &yesterday);
    return DateTime(t);
}

DateTime DateTime::GetNow()
{
    std::time_t now = std::time(0);
    tm t;
    localtime_s(&t, &now);
    return DateTime(t);
}

std::string FormatDateTimeAsString(const DateTime& time)
{
    std::string date = std::format("{}-{}-{}"
        , time.GetYear()
        , (time.GetMonth() < 10 ? "0" + std::to_string(time.GetMonth()) : std::to_string(time.GetMonth()))
        , (time.GetDay() < 10 ? "0" + std::to_string(time.GetDay()) : std::to_string(time.GetDay())));

    std::string timeStr = std::format("{}:{}"
        , time.GetHour()
        , (time.GetMinute() < 10 ? "0" + std::to_string(time.GetMinute()) : std::to_string(time.GetMinute())));

    return std::format("{}, {}", date, timeStr);
}


std::string FormatDateAsString(const DateTime& time)
{
    std::string date = std::format("{}-{}-{}"
        , time.GetYear()
        , (time.GetMonth() < 10 ? "0" + std::to_string(time.GetMonth()) : std::to_string(time.GetMonth()))
        , (time.GetDay() < 10 ? "0" + std::to_string(time.GetDay()) : std::to_string(time.GetDay())));
    return date;
}

DateTime DateTime::GetYesterdayDate()
{
    return GetYesterday();
}

DateTime DateTime::GetCurrentDate()
{
    return GetNow();
}

std::string DateTime::GetThisDay()
{
    int day = GetNow().GetDay();
    return std::string((day < 10 ? "0" + std::to_string(day) : std::to_string(day)));
}

std::string DateTime::GetThisMonth()
{
    int month = GetNow().GetMonth();
    return (month < 10 ? "0" + std::to_string(month) : std::to_string(month));
}

std::string DateTime::GetThisYear()
{
    int year = GetNow().GetYear();
    return (year < 10 ? "0" + std::to_string(year) : std::to_string(year));
}

END_NAMESPACE_DB
