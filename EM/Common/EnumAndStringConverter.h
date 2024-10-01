#pragma once

#include "EM/Common/CommonEnums.h"

#define MAP_ENTRY(enum_name, string_value) {enum_name::string_value, #string_value}

namespace em::common
{
	class EnumAndStringConverter
	{
	public:
		static const std::string& ConvertCmdTypeEnumToString(em::CmdType cmdType);

	private:
		inline static std::unordered_map<em::CmdType, std::string> s_MapCmdTypeEnumToString
		{
			MAP_ENTRY(em::CmdType, Help),
			MAP_ENTRY(em::CmdType, AddCategory),
			MAP_ENTRY(em::CmdType, Add),
			MAP_ENTRY(em::CmdType, Update),
			MAP_ENTRY(em::CmdType, Remove),
			MAP_ENTRY(em::CmdType, List),
			MAP_ENTRY(em::CmdType, Report),
			MAP_ENTRY(em::CmdType, CompareMonths),
			MAP_ENTRY(em::CmdType, SwitchAccount),
			MAP_ENTRY(em::CmdType, AddTags),
			MAP_ENTRY(em::CmdType, AddAccount),
			MAP_ENTRY(em::CmdType, RemoveAccount),
			MAP_ENTRY(em::CmdType, ClearScreen),
			MAP_ENTRY(em::CmdType, GitPush),
			MAP_ENTRY(em::CmdType, Invalid)
		};
	};
}


