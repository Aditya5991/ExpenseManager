#pragma once

BEGIN_NAMESPACE_DB

class Condition
{
public:

    enum class RelationshipType
    {
        AND,
        OR,
        INVALID
    };

    enum class Type
    {
        EQUALS,
        NOT_EQUALS,
        LIKE,
        NOT_LIKE,
        BETWEEN,

        INVALID
    };

    Condition(RelationshipType relationshipType = RelationshipType::AND) : Condition("", "", Type::INVALID, relationshipType) {}
    Condition(const std::string& name, const std::string& value, Type type, RelationshipType relationshipType = RelationshipType::INVALID);
    Condition(const std::string& name, const std::vector<std::string>& value, Type type, RelationshipType relationshipType = RelationshipType::INVALID);
    ~Condition();

    virtual std::string ToString() const;

    Condition& Add(Condition* cond);
    const std::string& GetName() const { return m_ParameterName; }
    const std::vector<std::string>& GetValue() const { return m_ParameterValue; }
    Type GetType() const { return m_Type; }
    bool IsValid() const { return (!GetName().empty() && !GetValue().empty()) || (m_Conditions.size() != 0); }
    void Reset() { *this = Condition(RelationshipType::AND); }
    void PopBack();

private:
    std::string GetValueInDbFormat(const std::string& value) const;

private:
    std::string m_ParameterName;
    std::vector<std::string> m_ParameterValue;
    Type        m_Type;
    RelationshipType            m_RelationshipType;
    std::vector<Condition*>    m_Conditions;

};

END_NAMESPACE_DB
