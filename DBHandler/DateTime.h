#pragma once

#include <ctime>

/**
* This class handles date functionality.
* Currently is restricted to only year, month, day in 'yyyy-mm-dd' format
*/

BEGIN_NAMESPACE_DB

class DateTime
{
public:
    DateTime(int year, int month, int day, char separator = '-');
    DateTime(const std::string& dateStr, char separator = '-');
    DateTime(tm t);

    bool operator () (const DateTime& d1, const DateTime& d2) const;
    bool operator < (const DateTime& other) const;
    bool operator > (const DateTime& other) const;
    bool operator == (const DateTime& other) const;

    // static
    static DateTime GetNow();
    static DateTime GetYesterday();
    static DateTime GetYesterdayDate();
    static DateTime GetCurrentDate();
    static std::string GetThisDay();
    static std::string GetThisMonth();
    static std::string GetThisYear();

    int GetSecond() const { return m_Second; }
    int GetMinute() const { return m_Minute; }
    int GetHour() const { return m_Hour; }
    int GetDay() const { return m_Day; }
    int GetMonth() const { return m_Month; }
    int GetYear() const { return m_Year; }

    std::string AsString(bool useTime = false) const;

private:
    int m_Second;
    int m_Minute;
    int m_Hour;
    int m_Day;
    int m_Month;
    int m_Year;
    char m_Separator;
};

END_NAMESPACE_DB