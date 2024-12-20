#include "pch.h"
#include "GeneralUtils.h"

namespace utils::general
{
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
            [](char c) { return !(isalnum(c) || (c == ' ')); }) == str.end();
    }

    bool IsAlphabets(const std::string& str)
    {
        return std::find_if(str.begin(), str.end(),
            [](char c) { return !isalnum(c); }) == str.end();
    }
}