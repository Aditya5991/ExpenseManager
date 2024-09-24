#include "Clause.h"

BEGIN_NAMESPACE_DB

Clause_OrderBy::Clause_OrderBy(
    const std::string& columnName, 
    OrderType type)
    : m_ColumnName(columnName)
    , m_Type(type)
{
}

bool Clause_OrderBy::IsDescending() const
{
    return m_Type == OrderType::DESCENDING;
}

bool Clause_OrderBy::IsValid() const
{
    return !m_ColumnName.empty();
}


END_NAMESPACE_DB
