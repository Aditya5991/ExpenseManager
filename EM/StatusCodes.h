#pragma once

#define ERROR_LOG(x, ...)   printf("\n<ERROR> %s </ERROR>" , std::format(x, __VA_ARGS__).c_str())

namespace em
{


    enum class StatusCode
    {
        Success,

        DisplayHelp,

        CommandDoesNotExist,
        InvalidNumMandatoryArgs,
        FlagDoesNotExist,
        ParameterNameDoesNotExist,
        InvalidParameterValue,

        CategoryDoesNotExist,
        TagDoesNotExist,

        DBError,

        JSONParsingError,

        NotImplementedYet,

        // Account Related
        AccountAlreadySelected,
        AccountDoesNotExist,

        CategoryAlreayExists,

        Invalid
    };

}