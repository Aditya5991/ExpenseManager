#pragma once

namespace em
{
    /**
    * List of supported commands by the application.
    */
    enum class CmdType
    {
        Help,
        AddCategory,
        Add,
        Update,
        Remove,
        List,
        Report,
        CompareMonths,
        SwitchAccount,
        AddTags,

        ClearScreen,

        GitPush,

        Invalid
    };

}

