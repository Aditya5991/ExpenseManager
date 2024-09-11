#include "EM/pch.h"
#include "GitPush.h"
#include "EM/Exceptions/General.h"
#include "DBHandler/Util.h"
#include "git2/git2.h"
#include "DBHandler/DateTime.h"

namespace em::action_handler::cli
{

    // protected
	em::action_handler::ResultSPtr GitPush::Execute(
		const std::string& commandName,
		const std::unordered_set<std::string>& flags,
		const std::map<std::string, std::string>& options)
	{
		assert(commandName == "git-push");
		system("git status");

		bool hasChanges = system("git diff --quiet") != 0;
		if (hasChanges)
		{
			std::cout << "Press 1 to push changes: ";
			int input = 0;
			std::cin >> input;
			if (input == 1)
			{
				system("git add .");
				std::string dateTimeStr = db::DateTime::GetNow().AsString();
				std::string commitMessage = std::format("Updated Database => {}", dateTimeStr);
				std::string commitCommand = std::format("git commit -m \"{}\"", commitMessage);
				system(commitCommand.c_str());
				system("git push origin main");

				std::cout << "Pushing Changes Done!" << std::endl;
			}
		}
		else
			std::cout << "No Changes in the repo";

        return em::action_handler::Result::Success();
	}
}