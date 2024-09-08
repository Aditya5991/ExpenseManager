#pragma once

BEGIN_NAMESPACE_DB


struct ForeignKeyReference
{
    std::string ColumnName; // name of the columns which is a foreign key
    std::string ReferenceTableName; // name of the table which the key references
    std::string AccessName; // name by which the db::Model the key is referencing to can be accessed
};

struct ColumnProperty
{
    std::string     Name;
    std::string     ValueType;
    bool            IsPrimaryKey;
    bool            IsForeignKey;
    bool            IsNotNull;
    bool            IsUnique;
    bool            AutoIncrement;

    ColumnProperty(
        const std::string& name, 
        const std::string& type, 
        bool isPrimaryKey = false, 
        bool isForeignKey = false,
        bool isNotNull = false, 
        bool isUnique = false, 
        bool autoIncrement = false)
        : Name(name)
        , ValueType(type)
        , IsPrimaryKey(isPrimaryKey)
        , IsForeignKey(isForeignKey)
        , IsNotNull(isNotNull)
        , IsUnique(isUnique)
        , AutoIncrement(autoIncrement)
    {
    }
};

struct ColumnProperty_RowID : public ColumnProperty
{
    ColumnProperty_RowID() : ColumnProperty("row_id", "integer", true, false, false, false, true)
    {
    }
};


END_NAMESPACE_DB