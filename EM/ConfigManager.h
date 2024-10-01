#pragma once

#include <optional>

namespace em
{
	typedef std::vector<std::string> ValidAccountNames;

	/**
	* This Singleton class handles everything related to the Configuration data
	* that will be contained in config.json file.
	*/
	class ConfigManager
	{
		/**
		* struct used to store config data.
		*/
		struct Data
		{
			std::optional<std::string> DefaultAccountName;
			std::string DefaultLocation;
		};

	public:
		ConfigManager();
		~ConfigManager();

		bool HasDefaultAccount() const;
		const std::string& GetDefaultAccount() const;
		const std::string& GetDefaultLocation() const;

		static ConfigManager& GetInstance();

	private:
		/**
		* This function will read the config.json file and store the relevant data.
		*/
		void Initialize();

		Data m_Data;
		static ConfigManager* s_Instance;
	};

}