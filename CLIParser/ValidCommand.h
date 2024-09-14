#pragma once

#include "Common.h"


BEGIN_NAMESPACE_CLI


struct ValidParameterProperties;
typedef std::unordered_map<std::string, ValidParameterProperties> ValidParamMap;


enum OptionType
{
    TEXT,
    ALPHA_NUMERIC,
    DOUBLE,
    INTEGER,
    ALPHABETICAL,
    DATE,

    INVALID
};


struct ValidParameterProperties
{
    ValidParameterProperties(
        OptionType type = OptionType::INVALID, 
        const std::string& helperMessage = "", 
        bool mandatory = false, 
        int index = -1,
        int numValues = 1)
        : Type(type)
        , HelperMessage(helperMessage)
        , IsMandatory(mandatory)
        , InOrderIndex(index)
        , NumValues(numValues){}

    ValidParameterProperties(const ValidParameterProperties& rhs) = default;

    OptionType  Type;
    bool        IsMandatory;
    int         InOrderIndex; // will be -1 for optional
    std::string HelperMessage;
    int         NumValues;
};


class ValidCommand
{
public:
    ValidCommand() {}
    ValidCommand(const std::string& name);

    ValidCommand&                           AddParameter(const std::string& name, const ValidParameterProperties& props);
    ValidCommand&                           AddFlag(const std::string& name, const std::string& helperMessage);
    bool                                    HasFlag(const std::string& flag) const;
    const std::string&                      GetName() const { return m_CommandName; }
    size_t                                  GetNumMandatoryParameters() const { return m_NumMandatoryParameters; }
    const ValidParamMap&                    GetParameters() const { return m_Parameters; }
    ValidParamMap::const_iterator           GetParameterAtIndex(size_t index) const;

    const std::unordered_map<std::string, std::string>&  GetFlags() const { return m_Flags; }

    static OptionType ConvertStringToOptionType(const std::string& optionType);

private:
    std::string                                     m_CommandName;
    std::unordered_map<std::string, std::string>    m_Flags;
    ValidParamMap                                   m_Parameters;
    size_t                                          m_NumMandatoryParameters;
};


END_NAMESPACE_CLI