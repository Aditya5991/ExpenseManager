#include "pch.h"
#include "DatabaseManager.h"
#include "Account/Account.h"
#include "Account/Manager.h"
#include "Utilities/Utils.h"
#include "ConfigManager.h"
#include "EM/Utils.h"
#include "DBHandler/Migration.h"
#include "AddCategoryIdForeignKeyMigration.h"

namespace em
{

    // private
    DatabaseManager* DatabaseManager::s_Instance = nullptr;
    bool DatabaseManager::s_IsInitialized = false;
    std::mutex DatabaseManager::s_Mutex;

    // public
    DatabaseManager& DatabaseManager::GetInstance()
    {
        if (!s_IsInitialized)
            DBG_ASSERT(!"DatabaseManager Not Initialized yet!");
        return *s_Instance;
    }

    // public
	DatabaseManager::~DatabaseManager()
	{
        delete s_Instance;
        s_Instance = nullptr;
	}

	// public
    DatabaseManager::DatabaseManager(const char* dbName, int openMode)
    {
        m_Database = std::make_unique<db::Database_SQLite>(dbName, openMode);
    }

    // public static
    void DatabaseManager::Create(const char* dbName, int openMode)
    {
        if (s_Instance == nullptr)
        {
            std::lock_guard<std::mutex> lk (s_Mutex);

            if (s_Instance == nullptr)
            {
                s_Instance = new DatabaseManager(dbName, openMode);
                s_IsInitialized = true;
            }

        }
    }

     // public
    void DatabaseManager::RunMigrations()
    {
        m_Database->RunMigration(AddCategoryIdForeignKeyMigration("AddCategoryIdForeignKey", "Added ForeignKey support, so updating expense tables!"));
    }

    // public
    void DatabaseManager::RegisterExpenseTables()
    {
        const std::filesystem::path& tableFolderPath = em::utils::GetDatabaseTableFolderPath();
        std::vector<std::filesystem::path> filePaths = ::utils::folder::GetFiles(tableFolderPath);
        for (const std::filesystem::path& path : filePaths)
            m_Database->CreateTableFromJson(path);
    }

    std::string DatabaseManager::GetCurrentExpenseTableName() const
    {
        std::shared_ptr<account::Account> account = em::account::Manager::GetInstance().GetCurrentAccount();
        const std::string& accountName = account->GetName();
        const std::string& tableName = accountName + "_expense";
        return tableName;
    }

    // public
    void DatabaseManager::OnSwitchAccount()
    {
        RegisterExpenseTables();
    }

    // public
    std::shared_ptr<db::Table> DatabaseManager::GetTable(const std::string& tableName) const
    {
        return m_Database->GetTable(tableName);
    }

}
