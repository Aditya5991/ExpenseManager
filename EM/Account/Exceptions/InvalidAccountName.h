#pragma once

#include "EM/Exceptions/Base.h"

namespace em::account::exceptions
{

    class InvalidAccountName : public em::exception::Base
    {
    public:

        InvalidAccountName(const std::string& accountName)
        {
            SetMessage(std::format("Invalid Account Name: {}", accountName));
        }
    };

}
