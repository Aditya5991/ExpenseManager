#include "pch.h"
#include "Utils.h"

BEGIN_NAMESPACE_UTIL

void SplitString(const std::string& s, char del, std::vector<std::string>& splits)
{
    std::stringstream ss(s);
    std::string word;
    while (!ss.eof())
    {
        getline(ss, word, del);
        splits.emplace_back(word);
    }
}

bool IsDouble(const std::string& str)
{
    return IsNumeric<double>(str);
}

bool IsInteger(const std::string& str)
{
    return IsNumeric<int>(str);
}

bool IsAlphaNumeric(const std::string& str)
{
    return IsDouble(str) || std::find_if(str.begin(), str.end(),
        [](char c) { return !(isalnum(c) || (c == ' ') || (c == '/')); }) == str.end();
}

bool IsDate(const std::string& str)
{
    std::vector<std::string> splits;
    SplitString(str, '-', splits);

    if (splits.size() != 3)
        return false;

    const std::string& year = splits[0];
    const std::string& month = splits[1];
    const std::string& day = splits[2];

    int d = std::atoi(day.c_str());
    if (d < 1 || d > 31)
        return false;

    int m = std::atoi(month.c_str());
    if (m < 1 || m > 12)
        return false;

    int y = std::atoi(year.c_str());
    if (y < 2022 || y > 3000)
        return false;

    return true;
}


END_NAMESPACE_UTIL
