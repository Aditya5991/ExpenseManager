#pragma once

#include "DBHandler/Condition.h"
#include "DBHandler/Util.h"

namespace em
{


    /**
    * This class represents the condition used to handle the filteration on 'list' command based on category.
    */
    class Condition_Category : public db::Condition
    {
    public:
        static db::Condition* Create(int categoryId)
        { 
            return new db::Condition("category_id", std::to_string(categoryId), db::Condition::Type::EQUALS);
        }
    };

    /**
    * This class represents the condition used to handle the filteration based on category name.
    */
    class Condition_CategoryName : public db::Condition
    {
    public:
        static db::Condition* Create(const std::string& categoryName)
        {
            return new db::Condition("name", categoryName, db::Condition::Type::EQUALS);
        }
    };

    /**
    * This class represents the condition used to handle the filteration on 'list' command based on date.
    */
    class Condition_Date : public db::Condition
    {
    public:
        static db::Condition* Create(const std::string& date)
        { 
            return new db::Condition("date", date, db::Condition::Type::EQUALS); 
        }
    };

    /**
    * Condition for current year's month.
    */
    class Condition_Month : public db::Condition
    {
    public:
        static db::Condition* Create(const std::string& month, const std::string year = db::util::GetThisYear())
        {
            return new db::Condition("date", std::format("{}-{}-%", year, month), db::Condition::Type::LIKE);
        }
    };

    /**
    * This class represents the condition used to handle the filteration on 'list' command based on year.
    */
    class Condition_Year : public db::Condition
    {
    public:
        static db::Condition* Create(const std::string& year)
        {
            return new db::Condition("date", std::format("{}-%", year), db::Condition::Type::LIKE);
        }
    };

    /**
    * This class represents the condition used to handle the filteration on 'list' command based on month and year.
    */
    class Condition_MonthAndYear : public db::Condition
    {
    public:
        static db::Condition* Create(const std::string& month, const std::string& year)
        {
            return new db::Condition("date", std::format("{}-{}-%", year, month), db::Condition::Type::LIKE);
        }
    };

    /**
    * This class represents the condition used to remove a row from table based on the rowID.
    */
    class Condition_DeleteRow : public db::Condition
    {
    public:
        static db::Condition* Create(int rowID)
        {
            return Create(std::to_string(rowID));
        }
        static db::Condition* Create(const std::string& rowID)
        {
            return new db::Condition("row_id", rowID, db::Condition::Type::EQUALS);
        }
    };


    /**
    * This class represents the condition used to handle the filteration on 'list' command based on name.
    */
    class Condition_ListNameFilter : public db::Condition
    {
    public:
        static db::Condition* Create(const std::string& name)
        {
            return new db::Condition("name", std::format("%{}%", name), db::Condition::Type::LIKE);
        }
    };

    /**
    * This class represents the condition used to handle the filteration on 'list' command based on location.
    */
    class Condition_LocationFilter : public db::Condition
    {
    public:
        static db::Condition* Create(const std::string& location)
        {
            return new db::Condition("location", location, db::Condition::Type::EQUALS);
        }
    };

    /**
    * This class represents the condition used to handle the filteration on 'list' and ignore the mentioned category.
    */
    class Condition_IgnoreCategory : public db::Condition
    {
    public:
        static db::Condition* Create(int categoryId)
        {
            return new db::Condition("category_id", std::to_string(categoryId), db::Condition::Type::NOT_EQUALS);
        }
    };

    /**
    * This class represents the condition used to handle the filteration on 'list' command based on tags.
    */
    class Condition_ListTagFilter : public db::Condition
    {
    public:
        static db::Condition* Create(const std::string& tagName)
        {
            std::string value = "";
            if (!tagName.empty())
                value = std::format("%{}%", tagName);
            return new db::Condition("tags", value, db::Condition::Type::LIKE);
        }
    };

    /**
    * This class represents the condition used to handle the filteration on 'list' and ignore the mentioned category.
    */
    class Condition_IgnoreTags : public db::Condition
    {
    public:
        static db::Condition* Create(const std::string& tagName)
        {
            std::string value = "";
            if (!tagName.empty())
                value = std::format("%{}%", tagName);
            return new db::Condition("tags", value, db::Condition::Type::NOT_LIKE);
        }
    };

    /**
    * This class represents the condition used to check if a tag exists in the Database.
    */
    class Condition_Tag : public db::Condition
    {
    public:
        static db::Condition* Create(const std::string& tagName)
        {
            return new db::Condition("name", std::format("%{}%", tagName), db::Condition::Type::LIKE);
        }
    };

}
