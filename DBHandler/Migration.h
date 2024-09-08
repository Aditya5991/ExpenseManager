#pragma once

#include "Common.h"

BEGIN_NAMESPACE_DB

class Database_SQLite;

/**
* Inherit from this class to create your own migration and override the Run() function.
* All the commands present in the Run() functions will be executed.
*/
class Migration
{
public:
    Migration(const std::string& name, const std::string& description = "")
        : m_Name(name)
    {
    }

    virtual bool Run(Database_SQLite& database) const = 0;
    const std::string& GetName() const { return m_Name; }
    const std::string& GetDescription() const { return m_Description; }

private:
    std::string m_Name;
    std::string m_Description;
};

END_NAMESPACE_DB
