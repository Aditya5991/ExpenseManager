#pragma once

#include "EM/ActionHandlers/IActionHandler.h"

namespace em
{
	enum class ReminderType
	{
		OneTime,
		Daily,
		Weekly,
		Monthly,
		Yearly,

		Invalid
	};
}
namespace em::action_handler::cli
{

	class AddReminder : public em::action_handler::Interface
	{

	private:

	public:
		/**
		* Same as em::action_handler::Interface::Execute(...)
		*/
		virtual em::action_handler::ResultSPtr Execute(
			const std::string& commandName,
			const std::unordered_set<std::string>& flags,
			const std::map<std::string, std::vector<std::string>>& options) override;

	private:

		/**
		* Decides whether a reminder can be created from the input data.
		* 
		* @params [in] type
		*		Type of the Reminder
		* 
		* @params [in] dateData
		*		Data required to create the reminder.
		*/
		bool Validate(ReminderType type, const std::string& dateData) const;

		/**
		* Converts std::string to enum class Type
		*/
		ReminderType GetTypeAsEnum(const std::string& enumAsStr) const;

	};

}

