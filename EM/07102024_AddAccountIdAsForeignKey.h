#pragma once

#include "DBHandler/Migration.h"
#include "DBHandler/Database_SQLite.h"

class AddAccountIdAsForeignKeyMigration : public db::Migration
{
public:

    AddAccountIdAsForeignKeyMigration()
        : db::Migration("AddAccountIdAsForeignKey", "Adding Account Id as a foreign Key to expense table!")
    {
    }

    virtual bool Run(db::Database_SQLite& database) const override
    {
        database.ExecQuery("alter table expenses add column account_id integer not null default -1;");
        database.ExecQuery("update expenses as ex set account_id = (select row_id from accounts where name = ex.account);");
        database.ExecQuery("alter table expenses drop column account;");
        return true;
    }
};
