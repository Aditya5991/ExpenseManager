#pragma once

namespace utils::general
{
    template<typename Numeric>
    bool IsNumeric(const std::string& str)
    {
        Numeric n;
        return((std::istringstream(str) >> n >> std::ws).eof());
    }

    bool IsDouble(const std::string& str);

    bool IsInteger(const std::string& str);

    bool IsAlphaNumeric(const std::string& str);

    bool IsAlphabets(const std::string& str);
}
