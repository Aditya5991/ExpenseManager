#include "pch.h"
#include "App.h"
#include "CLIParser/CLIParser.h"
#include "CLIParser/ValidCommand.h"
#include "CLIParser/Utils.h"
#include "EM/ActionHandlers/Cli/ActionHandlers.h"
#include "EM/DatabaseManager.h"
#include "EM/ConfigManager.h"
#include "EM/Exceptions/Config.h"
#include "EM/Exceptions/General.h"
#include "EM/Common/EnumAndStringConverter.h"
#include "EM/Account/Account.h"
#include "EM/Account/Manager.h"
#include "EM/Utils.h"
#include "JsonHelper/json.h"
#include "EM/ActionImplementor/Cli.h"

#include "DBHandler/Table.h"

#include <fstream>
#include <filesystem>

namespace em::app
{

	Interface::Interface(AppType appType)
		: m_AppType(appType)
		, m_IsFirstRun(false)
	{
	}

    // public
    action_handler::ResultSPtr Interface::PerformAction(const std::string& commandStr, CmdType cmdType)
    {
        return m_ActionImplementor->PerformAction(cmdType);
    }

	// protected
	bool Interface::InitializeDatabase()
	{
		try
		{
			std::filesystem::path configPath = em::utils::GetDatabaseConfigFilePath();

			Json::Value root;
			std::ifstream ifs(configPath);

			ifs >> root;

			std::string databaseName = root["name"].asString();

			em::DatabaseManager::Create(databaseName.c_str());

			em::DatabaseManager& dbMgr = em::DatabaseManager::GetInstance();
			dbMgr.RegisterExpenseTables();
			m_IsFirstRun = dbMgr.IsNewlyCreatedDatabase();
			dbMgr.RunMigrations();
		}
		catch (std::exception& e)
		{
			printf("\nFailed to Inititlize Database! Exception: %s", e.what());
			return false;
		}

		return true;
	}

}
