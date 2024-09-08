#pragma once


namespace em::action_handler
{
	struct Result;
	using ResultSPtr = std::shared_ptr<Result>;

	struct Result
	{
		StatusCode	statusCode;
		std::string message;

		bool IsSuccess() const { return statusCode == StatusCode::Success; }

		static ResultSPtr Success()
		{
			return std::make_shared<Result>(StatusCode::Success);
		}

		static ResultSPtr GeneralFailure(const std::string& message = "")
		{
			return std::make_shared<Result>(StatusCode::GeneralFailure, message);
		}

		static ResultSPtr Create(StatusCode statusCode, const std::string& message = "")
		{
			return std::make_shared<Result>(statusCode, message);
		}
	};


	class Interface
	{
	public:
		/**
		* This function will parse the json and call the appropriate Derived class Execute function.
		* 
		* @params [in] json
		*		json string representing all the details required to execute the command.
		* 
		* @return
		*		Object of ResultSPtr that contains the result details on execution of the function.
		*/
		ResultSPtr Execute(const std::string& json);

	protected:
		/**
		* This pure virtual function will be overwritten by the command specific action handler classes.
		* 
		* @params [in] commandName
		*		Name of the command that needs to be executed.
		* 
		* @params [in] flags
		*		Flags passed in by user.
		* 
		* @params [in] options
		*		Options passed in by user.
		* 
		* @return
		*		Object of ResultSPtr that contains the result details on execution of the function.
		*/
		virtual ResultSPtr Execute(
			const std::string& commandName,
			const std::unordered_set<std::string>& flags,
			const std::map<std::string, std::string>& options)
		{
			return em::action_handler::Result::Create(StatusCode::Invalid);
		}

	};

}


