#include "Database_SQLite.h"
#include "Table.h"
#include "SQLite_Database.h"
#include "JsonHelper/json.h"
#include "Util.h"
#include "QueryGenerator.h"

#include <fstream>

BEGIN_NAMESPACE_DB


Database_SQLite::Database_SQLite(
    const char* name, 
    int openMode)
    : m_OpenMode(openMode)
{
    m_DBImpl = new SQLite::Database(name, openMode);
    CreateMigrationTable();
}


Database_SQLite::Database_SQLite(
    const std::string& name, 
    int openMode)
    : Database_SQLite(name.c_str(), openMode)
{
}

Database_SQLite::~Database_SQLite()
{
    if (GetImpl())
    {
        delete GetImpl();
        m_DBImpl = nullptr;
    }
}

std::vector<ColumnProperty> Database_SQLite::GetMigrationTableColumnProperties()
{
    return
    {
        {
            "name",
            "text",
            true,
            false,
            true,
            true,
            false
        },
        {
            "description",
            "text"
        }
    };
}

void Database_SQLite::CreateMigrationTable()
{
    const std::vector<ColumnProperty> colProps = GetMigrationTableColumnProperties();
    auto migrationTable = std::make_shared<Table>(*this, "migrations", colProps);
    m_Tables.insert(std::make_pair(migrationTable->GetName(), migrationTable));
}

void Database_SQLite::RunMigration(const Migration& migration) 
{
    // check if the migration has already run
    auto table = GetTable("migrations");
    if (table->CheckIfExists("name", migration.GetName()))
        return;

    printf("\nRunning Migration: %s", migration.GetName().c_str());

    if (!migration.Run(*this))
    {
        printf("\nFailed to run migration: %s", migration.GetName().c_str());
        return;
    }

    Model model;
    model["name"] = migration.GetName();
    model["description"] = migration.GetDescription();
    table->Insert(model);
}


bool Database_SQLite::ExecQuery(const std::string& query)
{
    try 
    {
        SQLITE_LOG_QUERY(query.c_str());
        int numChanges = GetImpl()->exec(query);
        return numChanges > 0;
    }
    catch (SQLite::Exception& e)
    {
        SQLITE_EXCEPTION(e);
    }

    return false;
}

SQLite::Column Database_SQLite::ExecAndGet(const std::string& query)
{
    return GetImpl()->execAndGet(query);
}

std::shared_ptr<db::Table> Database_SQLite::CreateTableFromJson(const std::filesystem::path& filepath)
{
    try
    {
        Json::Value root;

        std::ifstream inputStream(filepath);
        inputStream >> root;

        const std::string& tableName = root["name"].asString();

        const Json::Value& columnPropsJson = root["columns"];

        std::vector<ColumnProperty> columnProps;
        for (const Json::Value& colJson : columnPropsJson)
        {
            const std::string& name = colJson["name"].asString();
            const std::string& valueType = colJson["valueType"].asString();
            bool isPrimaryKey = colJson.isMember("isPrimaryKey") ? colJson["isPrimaryKey"].asBool() : false;
            bool isForeignKey = colJson.isMember("isForeignKey") ? colJson["isForeignKey"].asBool() : false;
            bool isNotNull = colJson.isMember("isNotNull") ? colJson["isNotNull"].asBool() : false;
            bool isUnique = colJson.isMember("isUnique") ? colJson["isUnique"].asBool() : false;
            bool autoIncrement = colJson.isMember("autoIncrement") ? colJson["autoIncrement"].asBool() : false;

            columnProps.push_back(ColumnProperty(name, valueType, isPrimaryKey, isForeignKey, isNotNull, isUnique, autoIncrement));
        }

        const Json::Value& foreignKeyJson = root["foreignKeyReferences"];
        std::vector<ForeignKeyReference> foreignKeyReferences;
        for (const Json::Value& fkJson : foreignKeyJson)
        {
            ForeignKeyReference fkRef;
            fkRef.ColumnName = fkJson["columnName"].asString();
            fkRef.ReferenceTableName = fkJson["referenceTableName"].asString();
            fkRef.AccessName = fkJson["accessName"].asString();

            foreignKeyReferences.push_back(fkRef);
        }

        std::shared_ptr<db::Table> table = std::make_shared<db::Table>(*this, tableName, columnProps, foreignKeyReferences);
        m_Tables.insert(std::make_pair(tableName, table));
        return table;
    }
    catch (std::exception& ex)
    {
        printf("\nEXCEPTION: CreateTableFromJson: %s", ex.what());
        return nullptr;
    }

    return nullptr;
}

std::shared_ptr<db::Table> Database_SQLite::GetTable(const std::string& tableName)
{
    try
    {
        if (GetImpl()->tableExists(tableName) && m_Tables.find(tableName) != m_Tables.end())
            return m_Tables[tableName];
        else
        {
            printf("\nERROR: Table does not exist : %s", tableName.c_str());
            return nullptr;
        }
    }
    catch (std::exception& ex) 
    {
        printf("\nEXCEPTION: GetTable() - %s", ex.what());
        return nullptr;
    }
}

END_NAMESPACE_DB

