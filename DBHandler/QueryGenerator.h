#pragma once

#include "Common.h"


BEGIN_NAMESPACE_DB

class Condition;
class Table;
class Clause_OrderBy;
class Model;
struct ColumnProperty;
struct ForeignKeyReference;

class QueryGenerator
{
    static std::string FormatColumn(const ColumnProperty& prop);
    static std::string GetValueTypeAsString(std::string type);

public:
    static std::string AddColumnQuery(const std::string& tableName, const ColumnProperty& columnProp);
    static std::string CreateTableQuery(const std::string& tableName, const std::vector<ColumnProperty>& columns, const std::vector<ForeignKeyReference>& foreignKeyReferences = {});
    static std::string InsertQuery(const Table& table, const Model& model);
    static std::string UpdateQuery(const Table& table, const Model& origModel, const Model& newModel);
    static std::string SelectQuery(const Table& table, const Condition& condition, const Clause_OrderBy& orderBy);
    static std::string SumOfQuery(const Table& table, const std::string& columnName, const Condition& condition);
    static std::string DeleteQuery(const Table& table, const Condition& condition);
};

END_NAMESPACE_DB