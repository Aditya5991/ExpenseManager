#include "Table.h"
#include "Database_SQLite.h"
#include "Clause.h"
#include "JsonHelper/json.h"
#include "QueryGenerator.h"
#include "sqlite3.h"
#include <iostream>

BEGIN_NAMESPACE_DB

/**************************************************/
/*                  DBValue                      */
/**************************************************/

DBValue::DBValue(const std::any& value)
    : m_Value(value)
{
}

std::string DBValue::asString() const
{
    return std::any_cast<std::string>(m_Value);
}

double DBValue::asDouble() const
{
    return std::any_cast<double>(m_Value);
}

int DBValue::asInt() const
{
    return std::any_cast<int>(m_Value);
}

bool DBValue::asBool() const
{
    return std::any_cast<bool>(m_Value);
}

Model DBValue::asModel() const
{
    return std::any_cast<Model>(m_Value);
}

/**************************************************/
/*                 TableBase                      */
/**************************************************/

Table::Table(Database_SQLite& database, const std::string& tablename, const std::vector<ColumnProperty>& columnProps, const std::vector<ForeignKeyReference>& foreignKeyRefs)
    : m_Database(database)
    , m_Name(tablename)
    , m_ColumnProperties(columnProps)
    , m_ForeignKeyReferences(foreignKeyRefs)
{
    CreateTable();
    ValidateColumns();
}


bool Table::ExecQuery(const std::string& query)
{
    return GetDatabase().ExecQuery(query);
}

// protected
void Table::ValidateColumns()
{
    // if the number of columns have changed, alter the table to add new columns to database.
    const std::vector<std::string> columnNames = std::move(GetColumnNamesInDB());
    if (columnNames.size() == m_ColumnProperties.size())
        return;

    for (const ColumnProperty& colProp : m_ColumnProperties)
    {
        if (std::find(columnNames.begin(), columnNames.end(), colProp.Name) == columnNames.end())
        {
            std::string query = QueryGenerator::AddColumnQuery(m_Name, colProp);
            GetDatabase().ExecQuery(query);
        }
    }
}

// protected
void Table::CreateTable()
{
    if (!GetDatabase().GetImpl()->tableExists(m_Name))
    {
        std::string query = QueryGenerator::CreateTableQuery(m_Name, m_ColumnProperties, m_ForeignKeyReferences);
        GetDatabase().ExecQuery(query);
    }
}

double Table::SumOf(const std::string& columnName, const Condition& condition)
{
    std::string query = QueryGenerator::SumOfQuery(*this, columnName, condition);
    return GetDatabase().ExecAndGet(query).getDouble();
}

bool Table::Select(std::vector<Model>& rows, const Condition& condition, const Clause_OrderBy& orderBy) const
{
    try
    {
        std::string query = QueryGenerator::SelectQuery(*this, condition, orderBy);
        std::shared_ptr<SQLite::Statement> stmt = std::make_shared<SQLite::Statement>(*GetDatabase().GetImpl(), query);

        int count = 0;
        while (stmt->executeStep())
        {
            int index = 0;
            Model& row = rows.emplace_back();

            int columnCount = stmt->getColumnCount();
            while (index < columnCount)
            {
                SQLite::Column column = stmt->getColumn(index);

                const std::string& columnName = column.getName();

                ForeignKeyReference fkRef;
                if (IsForeignKey(columnName, &fkRef))
                {
                    auto fkTable = GetDatabase().GetTable(fkRef.ReferenceTableName);
                    Model fkModel;
                    fkTable->SelectById(fkModel, column.getInt());

                    row[fkRef.AccessName] = fkModel;
                    row[columnName] = column.getInt();
                }
                else
                {
                    int colType = column.getType();
                    switch (colType)
                    {
                    case SQLITE_INTEGER:
                        row[columnName] = std::any(column.getInt());
                        break;
                    case SQLITE_FLOAT:
                        row[columnName] = std::any(column.getDouble());
                        break;
                    case SQLITE_TEXT:
                        row[columnName] = std::any(column.getString());
                        break;
                    case SQLITE_NULL:
                        row[columnName] = std::any("");
                        break;
                    default:
                        assert(false);
                    }
                }


                ++index;
            }
        }

        return true;
    }
    catch (std::exception& ex)
    {
        printf("\nEXCEPTION: TableBase::Select - %s", ex.what());
        return false;
    }
}

bool Table::Select(Model& model, const Condition& condition, const Clause_OrderBy& orderBy) const
{
    std::vector<db::Model> rows;
    if (!Select(rows, condition, orderBy))
    {
        printf("\nNo Entry Found!");
        return false;
    }

    model = rows[0];
    return true;
}

bool Table::SelectById(Model& model, int id) const
{
    std::vector<Model> rows;
    if (!Select(rows, Condition("row_id", std::to_string(id), Condition::Type::EQUALS)))
        return false;

    model = rows[0];
    return true;
}

bool Table::Insert(const Model& model)
{
    const std::string& query = QueryGenerator::InsertQuery(*this, model);
    return ExecQuery(query);
}

bool Table::Update(const Model& origModel, const Model& newModel)
{
    const std::string& query = QueryGenerator::UpdateQuery(*this, origModel, newModel);
    return ExecQuery(query);
}

bool Table::Delete(const Condition& condition)
{
    const std::string& query = QueryGenerator::DeleteQuery(*this, condition);
    return ExecQuery(query);
}

bool Table::CheckIfExists(const std::string& columnName, const std::string& value, Model* model, Condition::Type compareType)
{
    if (columnName.empty())
        return false;

    std::vector<Model> rows;
    Select(rows, Condition(columnName, value, compareType));
    bool exists = rows.size() >= 1;
    if (model && exists)
        *model = rows[0];
    return exists;
}

bool Table::CheckIfExists(const Condition& condition)
{
    std::vector<Model> rows;
    Select(rows, condition);
    return rows.size() >= 1;
}

// protected
std::vector<std::string> Table::GetColumnNamesInDB() const
{
    std::vector<std::string> columnNames;
    std::string query = "SELECT * FROM " + m_Name + " LIMIT 1";
    SQLite::Statement stmt(*GetDatabase().GetImpl(), query);
    for (int i = 0; i < stmt.getColumnCount(); ++i)
        columnNames.push_back(stmt.getColumnName(i));
    return columnNames;
}

// public
bool Table::IsForeignKey(const std::string& columnName, ForeignKeyReference* fkRef) const
{
    auto iter = std::find_if(m_ForeignKeyReferences.begin(), m_ForeignKeyReferences.end(), [columnName, fkRef](ForeignKeyReference const& x) { return x.ColumnName == columnName; });
    if (iter == m_ForeignKeyReferences.end())
        return false;

    if (fkRef)
        *fkRef = *iter;
    return true;
}

bool Table::IsForeignKeyAccessName(const std::string& accessName, ForeignKeyReference* fkRef) const
{
    auto iter = std::find_if(m_ForeignKeyReferences.begin(), m_ForeignKeyReferences.end(), [accessName, fkRef](ForeignKeyReference const& x) { return x.AccessName == accessName; });
    if (iter == m_ForeignKeyReferences.end())
        return false;

    if (fkRef)
        *fkRef = *iter;
    return true;
}

bool Table::IsValidColumnName(const std::string& columnName) const
{
    return std::find_if(m_ColumnProperties.begin(), m_ColumnProperties.end(), [columnName] (ColumnProperty const& colProp){ return colProp.Name == columnName;})
            != m_ColumnProperties.end();
}

END_NAMESPACE_DB

