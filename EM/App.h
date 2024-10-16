#pragma once

#include "Common/CommonEnums.h"
#include "ActionHandlers/IActionHandler.h"
#include "ActionImplementor/IActionImplementor.h"

namespace em::app
{
	enum class AppType
	{
		CLI,
		SERVER,

		INVALID
	};

	class Interface
	{
	public:
		Interface(AppType appType);
		virtual void Start() = 0;
		virtual void Initialize() = 0;
		em::action_handler::ResultSPtr PerformAction(const std::string& commandStr, CmdType cmdType);

	protected:
		virtual void InitializeDatabase();
		virtual void InitializeAccountManager() = 0;
		virtual void InitializeActionImplementor() = 0;

	protected:
		AppType m_AppType;
		std::unique_ptr<em::action_impl::Interface> m_ActionImplementor;
	};

}
