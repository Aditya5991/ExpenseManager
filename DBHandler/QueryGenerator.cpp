#include "pch.h"
#include "QueryGenerator.h"
#include "SQLite_Database.h"
#include "Table.h"
#include "Database_SQLite.h"
#include "Util.h"

BEGIN_NAMESPACE_DB

// private
std::string QueryGenerator::GetValueTypeAsString(std::string type)\
{
    util::string::ToLower(type);
    if (type == "integer")
        return "INTEGER";

    if (type == "double")
        return "REAL";

    if (type == "text")
        return "TEXT";

    if (type == "date") // dates are stored as text in SQLITE
        return "TEXT";

    assert(false);
    return "";
}

// private 
std::string QueryGenerator::FormatColumn(const ColumnProperty& prop)
{
    return std::format("{} {} {} {} {} {}",
        prop.Name,
        GetValueTypeAsString(prop.ValueType),
        prop.IsPrimaryKey ? "PRIMARY KEY" : "",
        prop.IsNotNull ? "NOT NULL" : "",
        prop.IsUnique ? "UNIQUE" : "",
        prop.AutoIncrement ? "AUTOINCREMENT" : "");
}

// private
std::string QueryGenerator::AddColumnQuery(const std::string& tableName, const ColumnProperty& columnProp)
{
    std::string propStr = FormatColumn(columnProp);
    return std::format("ALTER TABLE {} ADD COLUMN {}", tableName, propStr);
}


std::string QueryGenerator::CreateTableQuery(
    const std::string& tableName, 
    const std::vector<ColumnProperty>& columns, 
    const std::vector<ForeignKeyReference>& foreignKeyReferences)
{
    std::ostringstream oss;
    oss << "CREATE TABLE IF NOT EXISTS " << tableName << "(";

    // process all the columns
    for (size_t i = 0; i < columns.size(); ++i)
    {
        const auto& prop = columns[i];
        std::string propStr = FormatColumn(prop);
        oss << propStr;

        if (i != columns.size() - 1)
            oss << ", ";
    }

    // process foreign keys
    for (size_t i = 0; i < foreignKeyReferences.size(); ++i)
    {
        if (i == 0)
            oss << ", ";

        const ForeignKeyReference& ref = foreignKeyReferences[i];
        oss << std::format("FOREIGN KEY ({}) REFERENCES {} (row_id)", ref.ColumnName, ref.ReferenceTableName);

        if (i != foreignKeyReferences.size() - 1)
            oss << ", ";
    }

    oss << ");";

    return oss.str();
}


std::string QueryGenerator::InsertQuery(const Table& table, const Model& model)
{
    std::ostringstream oss;
    oss << "INSERT INTO " << table.GetName();
    oss << " ( ";

    const std::vector<ColumnProperty>& columns = table.GetColumnProperties();
    size_t numColumns = columns.size();

    bool hasRowID = columns[0].Name == "row_id";
    size_t i = hasRowID ? 1 : 0;
    for (; i < columns.size(); ++i)
    {
        const auto& column = columns[i];

        oss << column.Name;
        if (i != numColumns - 1)
            oss << ", ";
    }

    oss << " ) VALUES ( ";

    i = hasRowID ? 1 : 0;
    for (; i < numColumns; ++i)
    {
        const ColumnProperty& colProp = columns[i];
        oss << "'";
        if (model.contains(colProp.Name)) // if the column value is not provided, enter empty value.
            oss << model.at(colProp.Name);
        oss << "'";
        if (i != numColumns - 1)
            oss << ", ";
    }

    oss << " );";

    return oss.str();
}

std::string QueryGenerator::UpdateQuery(const Table& table, const Model& origModel, const Model& newModel)
{
    std::ostringstream oss;
    oss << "UPDATE " << table.GetName() << " ";

    const std::vector<ColumnProperty>& columns = table.GetColumnProperties();
    size_t numColumns = columns.size();

    for (const ColumnProperty& column : columns)
    {
        if (origModel.at(column.Name) != newModel.at(column.Name))
        {
            oss << "SET " << column.Name << " = " << "'" << newModel.at(column.Name).asString() << "'" << " ";
            break;
        }
    }

    oss << "WHERE row_id = '" << origModel.at("row_id").asInt() << "'" << ";";
    return oss.str();
}

std::string QueryGenerator::SelectQuery(const Table& table, const Condition& condition, const Clause_OrderBy& orderBy)
{
    std::ostringstream oss;
    oss << "SELECT * from " << table.GetName();

    if (condition.IsValid())
    {
        oss << " WHERE ";
        oss << condition.ToString();
    }
    if (orderBy.IsValid())
    {
        oss << " ORDER BY ";
        oss << orderBy.GetColumnName();

        if (orderBy.IsDescending())
            oss << " DESC";
    }

    oss << ";";

    return oss.str();
}

std::string QueryGenerator::DeleteQuery(const Table& table, const Condition& condition)
{
    std::ostringstream oss;

    oss << "DELETE FROM " << table.GetName() << " ";
    oss << "WHERE " << condition.ToString();
    oss << ";";

    return oss.str();

}

std::string QueryGenerator::SumOfQuery(const Table& table, const std::string& columnName, const Condition& condition)
{
    std::ostringstream oss;

    oss << "select sum(" << columnName << ") ";
    oss << "from " << table.GetName();

    if (condition.IsValid())
    {
        oss << " WHERE ";
        oss << condition.ToString();
    }

    oss << ";";

    return oss.str();

}

END_NAMESPACE_DB
