#include "pch.h"
#include "ConfigManager.h"
#include "Utils.h"
#include "JsonHelper/json.h"
#include "Exceptions/Config.h"

#include <fstream>

namespace em
{
	ConfigManager* ConfigManager::s_Instance = nullptr;

	// public
	ConfigManager::ConfigManager()
	{
		Initialize();
	}

	ConfigManager::~ConfigManager()
	{
		delete s_Instance;
		s_Instance = nullptr;
	}

	// public
	bool ConfigManager::HasDefaultAccount() const
	{
		return m_Data.DefaultAccountName.has_value();
	}

	// public
	const std::string& ConfigManager::GetDefaultAccount() const
	{
		if (!HasDefaultAccount())
			throw std::exception("Default account name not present in config.json");

		return m_Data.DefaultAccountName.value();
	}

	// public
	const std::string& ConfigManager::GetDefaultLocation() const
	{
		return m_Data.DefaultLocation;
	}

	// private
	void ConfigManager::Initialize()
	{
		try
		{
			std::filesystem::path configFilePath = utils::GetConfigFilePath();
			std::ifstream inputStream(configFilePath);

			Json::Value root;
			inputStream >> root;

			if (!root.isMember("default"))
				return;

			// store default Account name
			if (root["default"].isMember("account"))
				m_Data.DefaultAccountName = root["default"]["account"].asString();

			// store default location
			if (root["default"].isMember("location"))
				m_Data.DefaultLocation = root["default"]["location"].asString();
		}
		catch (std::exception& e)
		{
			throw em::exception::Config(e.what());
		}
	}

	// public
	ConfigManager& ConfigManager::GetInstance()
	{
		if (s_Instance == nullptr)
			s_Instance = new ConfigManager();

		return *s_Instance;
	}

}