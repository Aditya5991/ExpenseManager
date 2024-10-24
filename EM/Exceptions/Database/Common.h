#pragma once

#include <format>

#include "EM/Exceptions/Base.h"

namespace em::exception
{
	class TableAlreadyRegistered : public em::exception::Base
	{

	public:
		TableAlreadyRegistered(const std::string& tablename)
		{
			SetMessage(std::format("Table Already Registered: {}", tablename));
		}
	};
}


