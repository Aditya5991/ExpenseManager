#pragma once

#include "../IActionHandler.h"

namespace db
{
	class Condition;
	class Clause_OrderBy;
	class Model;
	class DateTime;
}

namespace em::action_handler::cli
{

	class List : public em::action_handler::Interface
	{
	private:
		em::action_handler::ResultSPtr AppendCategoryCondition(db::Condition& finalCondition, const std::string& commaSeparatedCategories) const;
		em::action_handler::ResultSPtr AppendTagsCondition(db::Condition& finalCondition, const std::string& commaSeparatedTags) const;

	protected:
		/**
		* Same as em::action_handler::Interface::Execute(...)
		*/
		virtual em::action_handler::ResultSPtr Execute(
			const std::string& commandName,
			const std::unordered_set<std::string>& flags,
			const std::map<std::string, std::vector<std::string>>& options) override;

		/**
		* This function is used to handle the byDate flag
		* 
		* @params [in] cond
		*		Conditions that will be used to filter the data.
		*
		* @params [in] orderBy
		*		Determines ordering or the rows.
		* 
		* @return
		*		Object of ResultSPtr that contains the result details on execution of the function.
		*/
		virtual em::action_handler::ResultSPtr DisplayByDate(const db::Condition& cond, const db::Clause_OrderBy& orderBy) const;

		/**
		* This function retrieves the data from the Database Table, and renders it to the console based on the db::Condition and db::Clause_OrderBy.
		* 
		* @params [in] cond
		*		Conditions that will be used to filter the data.
		* 
		* @params [in] orderBy
		*		Determines ordering or the rows.
		*
		* @params [in] showTags
		*		Set true, to show the tags column.
		*
		* @params [in] showAccount
		*		Set true, to show the account column.
		*
		* @return
		*		Object of ResultSPtr that contains the result details on execution of the function.
		*
		*/
		virtual em::action_handler::ResultSPtr ProcessDBTable(
			const db::Condition& cond, 
			const db::Clause_OrderBy& orderBy,
			bool showTags = false,
			bool showAccount = false,
			bool showLocation = false);

		/**
		* This function retrieves the data from the Database Table, and renders it to the console based on the db::Condition and db::Clause_OrderBy
		* and the startDate, endDate passed.
		*
		* @params [in] cond
		*		Conditions that will be used to filter the data.
		*
		* @params [in] orderBy
		*		Determines ordering or the rows.
		* 
		* @params [in] startDate
		*		Determines the date above which the expenses are to be considered.
		*
		* @params [in] endDate
		*		Determines the date below which the expenses are to be considered.
		*
		* @params [in] showTags
		*		Set true, to show the tags column.
		*
		* @params [in] showAccount
		*		Set true, to show the account column.
		*
		* @return
		*		Object of ResultSPtr that contains the result details on execution of the function.
		*
		*/
		virtual em::action_handler::ResultSPtr ProcessDBTableWithDateRange(
			const db::Condition& cond,
			const db::Clause_OrderBy& orderBy,
			const db::DateTime& startDate,
			const db::DateTime& endDate,
			bool showTags = false,
			bool showAccount = false,
			bool showLocation = false);

		/**
		* This function returns the OrderBy clause for the query, which decides the ordering of the expenses in the table.
		* 
		* @params [in] orderByKey
		*		The key in DB by which we want to order the expenses.
		* 
		* @params [in] isDescending
		*		Flag to list the expenses in descending order.
		* 
		* @returns
		*		db::Clause_OrderBy object with the given params.
		*/
		db::Clause_OrderBy GetOrderByClause(const std::string& orderByKey , bool isDescending = false) const;

		/**
		* This function handles the request of listing categories
		* 
		* @return 
		*		Object of ResultSPtr that contains the result details on execution of the function.
		*/
		em::action_handler::ResultSPtr ListCategories();

		/**
		* This function handles the request of listing all the tags available in the database
		*
		* @return
		*		Object of ResultSPtr that contains the result details on execution of the function.
		*/
		em::action_handler::ResultSPtr ListTags();

		/**
		* This function handles the creation of the database condition to ignore tags from the records.
		* 
		* @params [in] commaSeparatedTagsToIgnore
		*		Comma separated strings of tags.
		* 
		* @returns
		*		Pointer to the new db::Condition object.
		*/
		db::Condition* CreateIgnoreTagsCondition(const std::string& commaSeparatedTagsToIgnore);

		/**
		* This helper function fills the 'rows' with the required expenses depending on 'cond' and 'orderBy'
		* 
		* @params [in] cond
		*		Conditions that will be used to filter the data.
		*
		* @params [in] orderBy
		*		Determines ordering or the rows.
		*/
		void GetExpenses(std::vector<db::Model>& rows, const db::Condition& cond, const db::Clause_OrderBy& orderBy) const;

	};

}

