#pragma once

#include "../IActionHandler.h"

namespace db
{
	class Condition;
	class Clause_OrderBy;
}

namespace em::action_handler::cli
{

	class GitPush : public em::action_handler::Interface
	{
	protected:
		/**
		* Same as em::action_handler::Interface::Execute(...)
		*/
		virtual em::action_handler::ResultSPtr Execute(
			const std::string& commandName,
			const std::unordered_set<std::string>& flags,
			const std::map<std::string, std::vector<std::string>>& options) override;
	};

}

