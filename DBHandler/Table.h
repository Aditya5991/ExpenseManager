#pragma once
#include "Common.h"
#include "Condition.h"
#include "ColumnProperty.h"
#include "Clause.h"

#include <format>
#include <any>


#define REGISTER_TABLE(tablename)   static const std::string GetName() { return tablename; }

BEGIN_NAMESPACE_DB


class Database_SQLite;
class Clause_OrderBy;
struct DBValue;


class Model : public std::unordered_map<std::string, DBValue>
{
};

struct DBValue
{
    DBValue() = default;
    DBValue(const std::any& value);

    std::string asString() const;
    double asDouble() const;
    int asInt() const;
    bool asBool() const;
    Model asModel() const;

    operator std::string()
    {
        return asString();
    }

    operator int()
    {
        return asInt();
    }

    DBValue operator[](const std::string& key) const
    {
        Model model = asModel();
        return model[key];
    }

    template<typename ValueType>
    void operator= (const ValueType& value)
    {
        m_Value = std::any(value);
    }

    template<typename T>
    bool operator == (const T& otherValue)
    {
        if (m_Value.type() != typeid(T))
            assert(false);

        return std::any_cast<T>(m_Value) == otherValue;
    }

    bool operator == (const DBValue& otherValue) const
    {
        if (m_Value.type() != otherValue.m_Value.type())
            return false;

        if (m_Value.type() == typeid(int))
        {
            return std::any_cast<int>(m_Value) == std::any_cast<int>(otherValue.m_Value);
        }
        else if (m_Value.type() == typeid(double))
        {
            return std::any_cast<double>(m_Value) == std::any_cast<double>(otherValue.m_Value);
        }
        else if (m_Value.type() == typeid(std::string))
        {
            return std::any_cast<std::string>(m_Value) == std::any_cast<std::string>(otherValue.m_Value);
        }
        else if (m_Value.type() == typeid(bool))
        {
            return std::any_cast<bool>(m_Value) == std::any_cast<bool>(otherValue.m_Value);
        }

        assert(false);
        return false;
    }

    bool operator != (const DBValue& otherValue) const
    {
        return !(*this == otherValue);
    }

    bool IsEmpty() const
    {
        return !m_Value.has_value();
    }

    friend std::ostringstream& operator<< (std::ostringstream& oss, const DBValue& value)
    {
        if (value.m_Value.type() == typeid(int))
            oss << value.asInt();
        else if (value.m_Value.type() == typeid(double))
            oss << value.asDouble();
        else if (value.m_Value.type() == typeid(std::string))
            oss << value.asString();
        else if (value.m_Value.type() == typeid(bool))
            oss << value.asBool();

        return oss;
    }

private:
    std::any m_Value;
};


/**
* RAII, will create the table in database on creating the Table object.
*/
class Table
{
public:
    Table(Database_SQLite& database, const std::string& tablename, const std::vector<ColumnProperty>& columnProps, const std::vector<ForeignKeyReference>& foreignKeyRefs = {});
    virtual ~Table() = default;

    const std::string& GetName() const { return m_Name; }

    bool ExecQuery(const std::string& query);

    double SumOf(const std::string& columnName, const Condition& condition = Condition());
    bool CheckIfExists(
        const std::string& columnName, 
        const std::string& value, 
        Model* model = nullptr, 
        Condition::Type compareType = Condition::Type::EQUALS);
    bool CheckIfExists(const Condition& condition);
    bool Select(std::vector<Model>& rows, const Condition& condition = Condition(), const Clause_OrderBy& orderBy = Clause_OrderBy()) const;
    bool Select(Model& model, const Condition& condition = Condition(), const Clause_OrderBy& orderBy = Clause_OrderBy()) const;
    bool SelectById(Model& model, int id) const;
    bool Insert(const Model& model);
    bool Update(const Model& origModel, const Model& model);
    bool Delete(const Condition& condition);
    bool IsForeignKey(const std::string& columnName, ForeignKeyReference* fkRef = nullptr) const;
    bool IsForeignKeyAccessName(const std::string& accessName, ForeignKeyReference* fkRef = nullptr) const;
    bool IsValidColumnName(const std::string& columnName) const;

    void SetColumnProperties(const std::vector<ColumnProperty>& columns) { m_ColumnProperties = columns; }
    const std::vector<ColumnProperty>& GetColumnProperties() const { return m_ColumnProperties; }
    Database_SQLite& GetDatabase() const { return m_Database; }

protected:
    void CreateTable();
    void ValidateColumns();
    std::vector<std::string> GetColumnNamesInDB() const;


protected:
    Database_SQLite&            m_Database;
    std::string                 m_Name;
    std::vector<ColumnProperty> m_ColumnProperties;
    std::vector<ForeignKeyReference> m_ForeignKeyReferences;
};

END_NAMESPACE_DB


