#include "pch.h"
#include "DatabaseManager.h"
#include "Utilities/Utils.h"
#include "ConfigManager.h"
#include "EM/Utils.h"
#include "DBHandler/Table.h"
#include "DBHandler/Migration.h"
#include "Migrations.h"
#include "Utilities/FileUtils.h"

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
        std::filesystem::path dbFilepath = utils::GetExecutableDirPath() / dbName;
        m_IsNewlyCreatedDatabase = !(::utils::file::Exists(dbFilepath));
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
        bool isFirstRun = m_IsNewlyCreatedDatabase;
        m_Database->RunMigration(AddCategoryIdForeignKeyMigration(), isFirstRun);
        m_Database->RunMigration(CombineAllExpenseTableIntoOneMigration(), isFirstRun);
        m_Database->RunMigration(AddAccountTableMigration(), isFirstRun);
        m_Database->RunMigration(AddAccountIdAsForeignKeyMigration(), isFirstRun);
    }

    // public
    void DatabaseManager::RegisterExpenseTables()
    {
        const std::filesystem::path& tableFolderPath = em::utils::GetDatabaseTableFolderPath();
        std::vector<std::filesystem::path> filePaths = ::utils::folder::GetFiles(tableFolderPath);
        for (const std::filesystem::path& path : filePaths)
            m_Database->CreateTableFromJson(path);
    }

    // public
    bool DatabaseManager::IsNewlyCreatedDatabase() const
    {
        return m_IsNewlyCreatedDatabase;
    }

    // public
    std::shared_ptr<db::Table> DatabaseManager::GetTable(const std::string& tableName) const
    {
        return m_Database->GetTable(tableName);
    }

}
