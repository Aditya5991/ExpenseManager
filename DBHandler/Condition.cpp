#include "Common.h"
#include "Condition.h"

BEGIN_NAMESPACE_DB


Condition::Condition(
    const std::string& name, 
    const std::string& value, 
    Type type, 
    RelationshipType relationshipType)
    : m_ParameterName(name)
    , m_ParameterValue(std::vector<std::string>{ value })
    , m_Type(type)
    , m_RelationshipType(relationshipType)
{
}

Condition::Condition(
    const std::string& name, 
    const std::vector<std::string>& value, 
    Type type, 
    RelationshipType relationshipType)
    : m_ParameterName(name)
    , m_ParameterValue(value)
    , m_Type(type)
    , m_RelationshipType(relationshipType)
{
}

Condition::~Condition()
{
    for (Condition* cond : m_Conditions)
    {
        delete cond;
        cond = nullptr;
    }
}

Condition& Condition::Add(Condition* cond)
{
    m_Conditions.push_back(cond);
    return *this;
}

std::string Condition::ToString() const
{
    if (m_Conditions.empty())
    {
        std::ostringstream oss;

        std::string oper;
        switch (GetType())
        {
        case Type::EQUALS:
            oper = " = ";
            break;
        case Type::NOT_EQUALS:
            oper = " != ";
            break;
        case Type::LIKE:
            oper = " LIKE ";
            break;
        case Type::NOT_LIKE:
            oper = " NOT LIKE ";
        case Type::BETWEEN:
            oper = " BETWEEN ";
            break;
        }

        if (GetType() == Type::BETWEEN)
        {
            oss << GetName() << oper << GetValueInDbFormat(GetValue().front()) << " AND " << GetValueInDbFormat(GetValue().back());
        }
        else
        {
            oss << GetName() << oper << GetValueInDbFormat(GetValue().front());
        }

        return oss.str();
    }

    std::string query = "(";
    std::string relationshipStr = m_RelationshipType == RelationshipType::AND ? " AND " : " OR ";

    for (const auto& condition : m_Conditions) {
        query += condition->ToString() + relationshipStr;
    }

    // Remove the last relationship string
    query = query.substr(0, query.size() - relationshipStr.size());

    query += ")";

    return query;
}

void Condition::PopBack()
{
    m_Conditions.pop_back();
}

std::string Condition::GetValueInDbFormat(const std::string& value) const
{
    return std::format("'{}'", value);
}

END_NAMESPACE_DB