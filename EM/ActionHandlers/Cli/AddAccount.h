#pragma once

#include "../IActionHandler.h"

namespace em::action_handler::cli
{

	class AddAccount : public em::action_handler::Interface
	{
	public:
		/**
		* Same as em::action_handler::Interface::Execute(...)
		*/
		virtual em::action_handler::ResultSPtr Execute(
			const std::string& commandName,
			const std::unordered_set<std::string>& flags,
			const std::map<std::string, std::vector<std::string>>& options) override;
	};

}

