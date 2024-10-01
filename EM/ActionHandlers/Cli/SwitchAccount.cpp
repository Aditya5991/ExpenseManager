#include "EM/pch.h"
#include "SwitchAccount.h"
#include "../../Account/Manager.h"
#include "../../DatabaseManager.h"
#include "../../ActionImplementor.h"

namespace em::action_handler::cli
{
	em::action_handler::ResultSPtr SwitchAccount::Execute(
		const std::string& commandName,
		const std::unordered_set<std::string>& flags,
		const std::map<std::string, std::vector<std::string>>& options)
	{
		DBG_ASSERT(commandName == "switchAccount");

		const std::string& newAccountName = options.at("accountName").front();
		StatusCode switchStatus = em::account::Manager::GetInstance().OnSwitchAccount(newAccountName);

		// handle if account not switched.
		if (switchStatus != StatusCode::Success)
		{
			const std::string& errorMessage = GetErrorMessage(switchStatus, newAccountName);
			return em::action_handler::Result::Create(switchStatus, errorMessage);
		}

		printf("\nAccount Switched to : %s", newAccountName.c_str());
		return em::action_handler::Result::Success();
	}

	std::string SwitchAccount::GetErrorMessage(StatusCode statusCode, const std::string& newAccountName) const
	{
		switch (statusCode)
		{
		case StatusCode::AccountDoesNotExist:
			return std::format(ERROR_ACCOUNT_DOES_NOT_EXIST, newAccountName);
		case StatusCode::AccountAlreadySelected:
			return std::format(ERROR_ACCOUNT_ALREADY_SELECTED, newAccountName);
		}

		assert(false);
		return "";
	}
}
