#pragma once

#include "DBHandler/Migration.h"
#include "DBHandler/Database_SQLite.h"

/**
* Account table will be added in subsequent migrations.
* Account will belong to a person.
*/
class CombineAllExpenseTableIntoOneMigration : public db::Migration
{
public:

    CombineAllExpenseTableIntoOneMigration()
        : db::Migration("CombineAllExpenseTableIntoOne", "Combined all 3 tables into a single expenses table!")
    {
    }

    virtual bool Run(db::Database_SQLite& database) const override
    {
        database.ExecQuery("INSERT INTO tags(name) VALUES ('Marriage');");
        database.ExecQuery("CREATE TABLE expenses(row_id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, price REAL NOT NULL, date TEXT, location TEXT NOT NULL, tags text, category_id integer, account text NOT NULL);");
        database.ExecQuery("INSERT INTO expenses(name, price, date, location, tags, category_id, account) SELECT name, price, date, location, tags, category_id, 'aditya' FROM household_expense;");
        database.ExecQuery("INSERT INTO expenses(name, price, date, location, tags, category_id, account) SELECT name, price, date, location, tags, category_id, 'aditya' FROM personal_expense;");
        database.ExecQuery("INSERT INTO expenses(name, price, date, location, tags, category_id, account) SELECT name, price, date, location, 'Marriage', category_id, 'aditya' FROM marriage_expense;");
        database.ExecQuery("DROP TABLE household_expense;");
        database.ExecQuery("DROP TABLE personal_expense;");
        database.ExecQuery("DROP TABLE marriage_expense;");

        return true;
    }
};
