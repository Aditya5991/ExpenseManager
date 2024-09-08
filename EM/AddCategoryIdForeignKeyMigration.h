#pragma once

#include "DBHandler/Migration.h"
#include "DBHandler/Database_SQLite.h"

class AddCategoryIdForeignKeyMigration : public db::Migration
{
public:

    AddCategoryIdForeignKeyMigration(const std::string& name, const std::string& description)
        : db::Migration(name, description)
    {
    }

    virtual bool Run(db::Database_SQLite& database) const override
    { 
        RunForTable(database, "household_expense");
        RunForTable(database, "personal_expense");
        RunForTable(database, "marriage_expense");
        return true;
    }

private:

    void RunForTable(db::Database_SQLite& database, const std::string& tableName) const
    {
        database.ExecQuery(std::format("alter table {} add column category_id integer", tableName));
        database.ExecQuery(std::format("update {} as hs set category_id = (select row_id from categories where name = hs.category)", tableName));
        database.ExecQuery(std::format("alter table {} drop column category", tableName));
    }
};
