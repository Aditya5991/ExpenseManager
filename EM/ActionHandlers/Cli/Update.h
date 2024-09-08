#pragma once

#include "../IActionHandler.h"

namespace em::action_handler::cli
{

	class Update : public em::action_handler::Interface
	{
	public:
		/**
		* Same as em::action_handler::Interface::Execute(...)
		*/
		virtual em::action_handler::ResultSPtr Execute(
			const std::string& commandName,
			const std::unordered_set<std::string>& flags,
			const std::map<std::string, std::string>& options) override;

	private:
		/**
		* This function validates the 'atteibuteName' and 'attributeValue' params
		*/
		ResultSPtr Validate(const std::string& attributeName, const std::string& attributeValue);

		/**
		* This function handles the case where we are updating a parameter that is a foreign key. e.g. category.
		*/
		void HandleForeignKeyUpdate(std::string& attributeName, std::string& attributeValue);

	};

}

