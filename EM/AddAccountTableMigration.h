#pragma once

#include "DBHandler/Migration.h"
#include "DBHandler/Database_SQLite.h"

class AddAccountTableMigration : public db::Migration
{
public:

    AddAccountTableMigration()
        : db::Migration("AddAccountTable", "Adding Account table to keep track of registered account!")
    {
    }

    virtual bool Run(db::Database_SQLite& database) const override
    {
        database.ExecQuery("CREATE TABLE accounts(row_id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, description TEXT);");
        database.ExecQuery("INSERT INTO accounts (name, description) VALUES ('aditya', '');");
        return true;
    }
};
