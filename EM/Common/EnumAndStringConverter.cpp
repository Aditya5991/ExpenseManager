#include "EM/pch.h"
#include "EnumAndStringConverter.h"
#include "EM/Exceptions/EnumStringConverter.h"


namespace em::common
{

	const std::string& EnumAndStringConverter::ConvertCmdTypeEnumToString(em::CmdType cmdType)
	{
		auto iter = s_MapCmdTypeEnumToString.find(cmdType);
		if (iter == s_MapCmdTypeEnumToString.end())
			throw em::exception::EnumStringConverter("em::CmdType", "std::string");

		return iter->second;
	}

}
