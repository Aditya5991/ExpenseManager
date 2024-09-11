#include "pch.h"
#include "Util.h"

BEGIN_NAMESPACE_DB_UTIL

namespace string
{

    void ToLower(std::string& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), [](char ch) { return std::tolower(ch); });
    }

}

END_NAMESPACE_DB_UTIL
