#pragma once

#include "Common.h"
#include "SQLite_Database.h"
#include "Migration.h"

BEGIN_NAMESPACE_DB

const int OK = SQLite::OK;
const int OPEN_READONLY = SQLite::OPEN_READONLY;
const int OPEN_READWRITE = SQLite::OPEN_READWRITE;
const int OPEN_CREATE = SQLite::OPEN_CREATE;

struct ColumnProperty;
class Table;
typedef std::shared_ptr<Table> TableSPtr;


class Database_SQLite
{
public:
    Database_SQLite(const char* name, int openMode = OPEN_CREATE | OPEN_READWRITE);
    Database_SQLite(const std::string& name, int openMode = OPEN_CREATE | OPEN_READWRITE);

    Database_SQLite(const Database_SQLite&) = delete;
    Database_SQLite& operator=(const Database_SQLite&) = delete;

    virtual ~Database_SQLite();

    void operator delete(void* ptr, size_t size)
    {
        ::delete ptr;
        ptr = nullptr;
    }

    std::shared_ptr<db::Table> CreateTableFromJson(const std::filesystem::path& filepath);
    std::shared_ptr<db::Table> GetTable(const std::string& tableName);

    /**
    * This function checks if the migration has already been run, if no, then it will call the Migration::Run() function
    * and execute all the commands present there
    */
    void RunMigration(const Migration& migration);

    // queries
    SQLite::Column ExecAndGet(const std::string& query);
    bool ExecQuery(const std::string& query);

    const std::string GetName() const { return m_DBImpl->getFilename(); }
    int GetOpenMode() const { return m_OpenMode; }

private:
    std::vector<ColumnProperty> GetMigrationTableColumnProperties();
    void CreateMigrationTable();

    SQLite::Database* GetImpl() { return m_DBImpl; }
    const SQLite::Database* GetImpl() const { return m_DBImpl; }

private:
    int                                             m_OpenMode;
    SQLite::Database*                               m_DBImpl;
    std::unordered_map<std::string, std::shared_ptr<db::Table>> m_Tables;

    friend class Table;
};


END_NAMESPACE_DB
