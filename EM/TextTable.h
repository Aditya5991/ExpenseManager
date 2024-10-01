#pragma once

#include <iomanip>
#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>

#include "ReportHandler.h"
#include "Utils.h"
#include "DBHandler/Table.h"
#include "DBHandler/DateTime.h"

#ifdef TEXTTABLE_ENCODE_MULTIBYTE_STRINGS
#include <clocale>
#ifndef TEXTTABLE_USE_EN_US_UTF8
#define TEXTTABLE_USE_EN_US_UTF8
#endif
#endif

namespace em
{
    /**
    * This class can be used to create a table for representing data on the CLI.
    */
    class TextTable
    {
    public:
        enum class Alignment { LEFT, RIGHT };
        typedef std::vector<std::string> Row;
        TextTable()
            : _horizontal('-'), _vertical('|'), _corner('+'), _has_ruler(true) {}

        TextTable(char horizontal, char vertical, char corner)
            : _horizontal(horizontal), _vertical(vertical), _corner(corner),
            _has_ruler(true) {}

        explicit TextTable(char vertical)
            : _horizontal('\0'), _vertical(vertical), _corner('\0'),
            _has_ruler(false) {}

        void setAlignment(unsigned i, Alignment alignment) {
            _alignment[i] = alignment;
        }

        virtual void Print()
        {
            if (rows().size() < 1)
                return;

            printf("\n%s", toString().c_str());
        }

        Alignment alignment(unsigned i) const { return _alignment[i]; }

        char vertical() const { return _vertical; }

        char horizontal() const { return _horizontal; }

        TextTable& add(const std::string& content) { _current.push_back(content); return *this; }

        void endOfRow() {
            _rows.push_back(_current);
            _current.assign(0, "");
        }

        template <typename Iterator> void addRow(Iterator begin, Iterator end) {
            for (auto i = begin; i != end; ++i) {
                add(*i);
            }
            endOfRow();
        }

        template <typename Container> void addRow(const Container& container) {
            addRow(container.begin(), container.end());
        }

        const std::vector<Row>& rows() const { return _rows; }

        void setup() const {
            determineWidths();
            setupAlignment();
        }

        std::string ruler() const {
            std::string result;
            result += _corner;
            for (auto width = _width.begin(); width != _width.end(); ++width) {
                result += repeat(*width, _horizontal);
                result += _corner;
            }

            return result;
        }

        int width(unsigned i) const { return _width[i]; }

        bool has_ruler() const { return _has_ruler; }

        int correctDistance(const std::string& string_to_correct) const {
            return static_cast<int>(string_to_correct.size()) -
                static_cast<int>(glyphLength(string_to_correct));
        };

    private:
        const char _horizontal;
        const char _vertical;
        const char _corner;
        const bool _has_ruler;
        Row _current;
        std::vector<Row> _rows;
        std::vector<unsigned> mutable _width;
        std::vector<unsigned> mutable _utf8width;
        std::map<unsigned, Alignment> mutable _alignment;

        static std::string repeat(unsigned times, char c) {
            std::string result;
            for (; times > 0; --times)
                result += c;

            return result;
        }

        unsigned columns() const { return static_cast<unsigned>(_rows[0].size()); }

        unsigned glyphLength(const std::string& s) const {
            unsigned int _byteLength = static_cast<unsigned int>(s.length());
#ifdef TEXTTABLE_ENCODE_MULTIBYTE_STRINGS
#ifdef TEXTTABLE_USE_EN_US_UTF8
            std::setlocale(LC_ALL, "en_US.utf8");
#else
#error You need to specify the encoding if the TextTable library uses multybyte string encoding!
#endif
            unsigned int u = 0;
            const char* c_str = s.c_str();
            unsigned _glyphLength = 0;
            while (u < _byteLength) {
                u += std::mblen(&c_str[u], _byteLength - u);
                _glyphLength += 1;
            }
            return _glyphLength;
#else
            return _byteLength;
#endif
        }

        void determineWidths() const {
            if (_rows.empty()) {
                return;
            }
            _width.assign(columns(), 0);
            _utf8width.assign(columns(), 0);
            for (auto rowIterator = _rows.begin(); rowIterator != _rows.end();
                ++rowIterator) {
                Row const& row = *rowIterator;
                for (unsigned i = 0; i < row.size(); ++i) {
                    _width[i] =
                        _width[i] > glyphLength(row[i]) ? _width[i] : glyphLength(row[i]);
                }
            }
        }

        void setupAlignment() const {
            if (_rows.empty()) {
                return;
            }
            for (unsigned i = 0; i < columns(); ++i) {
                if (_alignment.find(i) == _alignment.end()) {
                    _alignment[i] = Alignment::LEFT;
                }
            }
        }

    public:
        std::string toString()
        {
            const TextTable& table = *this;
            std::ostringstream oss;
            if (table.rows().empty()) {
                return oss.str();
            }
            table.setup();
            if (table.has_ruler()) {
                oss << table.ruler() << "\n";
            }
            for (auto rowIterator = table.rows().begin();
                rowIterator != table.rows().end(); ++rowIterator) {
                TextTable::Row const& row = *rowIterator;
                oss << table.vertical();
                for (unsigned i = 0; i < row.size(); ++i) {
                    auto alignment = table.alignment(i) == TextTable::Alignment::LEFT
                        ? std::left
                        : std::right;

                    // std::setw( width ) works as follows: a string which goes in the stream
                    // with byte length (!) l is filled with n spaces so that l+n=width. For a
                    // utf8 encoded string the glyph length g might be smaller than l. We need
                    // n spaces so that g+n=width which is equivalent to g+n+l-l=width ==> l+n
                    // = width+l-g l-g (that means glyph length minus byte length) has to be
                    // added to the width argument. l-g is computed by correctDistance.
                    oss << std::setw(table.width(i) + table.correctDistance(row[i]))
                        << alignment << row[i];
                    oss << table.vertical();
                }
                oss << "\n";
                if (table.has_ruler()) {
                    oss << table.ruler() << "\n";
                }
            }

            return oss.str();
        }
    };


    class TextTable_TotalExpense : public TextTable
    {
    public:
        TextTable_TotalExpense(double total)
            : TextTable()
        {
            add("TOTAL").add(std::to_string(total)).endOfRow();
        }
    };


    class TextTable_Expense : public TextTable
    {
    public:
        /**
        * @params [in] accountName
        *       Name of the account for which we are rendering the records.
        *
        * @params [in] records
        *       Vector of records for the given account.
        */
        TextTable_Expense(
            const std::string& accountName, 
            const std::vector<db::Model>& records, 
            bool showTags = false, 
            bool showAccount = false,
            bool showLocation = false)
            : TextTable()
            , m_ShowTags(showTags)
            , m_ShowAccount(showAccount)
            , m_ShowLocation(showLocation)
        {
            AppendHeader();
            AppendRows(records, accountName);
        }

        /**
        * @params [in] rows
        *       A map with key as the accountName that the expenses belong to, and value as the actual expense records for the given account.
        */
        TextTable_Expense(const std::unordered_map<std::string, std::vector<db::Model>>& rows)
            : TextTable()
        {
            AppendHeader();
            for (auto iter = rows.begin(); iter != rows.end(); iter++)
            {
                const std::string& accountName = iter->first;
                const std::vector<db::Model> records = iter->second;

                AppendRows(records, accountName);
            }
        }

    private:
        bool m_ShowTags;
        bool m_ShowAccount;
        bool m_ShowLocation;

    private:
        void AppendHeader()
        {
            add("ROW_ID").add("NAME").add("CATEGORY").add("PRICE").add("DATE");
            
            if(m_ShowLocation)
                add("LOCATION");

            if (m_ShowTags)
                add("TAGS");

            if (m_ShowAccount)
                add("ACCOUNT");

            endOfRow();
        }

        void AppendRows(const std::vector<db::Model>& rows, const std::string& accountName)
        {
            if (rows.size() < 1)
                return;

            for (const db::Model& row : rows)
            {
                add(std::to_string(row.at("row_id").asInt()));
                add(row.at("name").asString());
                add(row.at("category")["name"].asString());
                add(std::to_string(row.at("price").asDouble()));
                add(row.at("date").asString());

                if(m_ShowLocation)
                    add(row.at("location").asString());

                if (m_ShowTags)
                {
                    auto temp = row.at("tags");
                    add(row.at("tags").asString());
                }
                if(m_ShowAccount)
                    add(accountName);

                endOfRow();
            }
        }
    };

    class TextTable_Expense_ByDate : public TextTable
    {
    public:
        TextTable_Expense_ByDate(const std::map<db::DateTime, double>& pricesByDate)
            : TextTable()
        {
            add("DATE").add("PRICE").endOfRow();
            for (auto [date, price] : pricesByDate)
                add(date.AsString()).add(std::to_string(price)).endOfRow();
        }
    };

    class TextTable_Category : public TextTable
    {
    public:
        TextTable_Category(const std::vector<db::Model>& rows)
            : TextTable()
            , m_Rows(rows)
        {
            add("ROW_ID").add("NAME").endOfRow();
            for (const db::Model& row : m_Rows)
            {
                add(std::to_string(row.at("row_id").asInt()))
                    .add(row.at("name").asString())
                    .endOfRow();
            }
        }

    private:
        const std::vector<db::Model>& m_Rows;
    };


    class TextTable_Report : public TextTable
    {
    public:

        TextTable_Report(const std::vector<std::pair<std::string, double>>& prices)
            : TextTable()
            , m_Prices(prices)
        {

            add("Category").add("Total").endOfRow();
            for (const auto& [categoryName, price] : prices)
            {
                add(categoryName)
                    .add(std::to_string(price))
                    .endOfRow();
            }
        }

    private:
        const std::vector<std::pair<std::string, double>>& m_Prices;
    };


    class TextTable_CompareReport : public TextTable
    {
    public:

        TextTable_CompareReport(const std::vector<ReportHandler>& reports)
        {
            // header
            AddHeader(reports);

            std::vector<double> totalPrices(reports.size(), 0.0);
            const auto& prices = reports[0].GetPrices();
            // iterate through each category
            for (auto iter = prices.begin(); iter != prices.end(); iter++)
            {
                const std::string& categoryName = iter->first;
                add(categoryName);

                // iterate through each report and get the price for the current category
                for (size_t i = 0; i < reports.size(); ++i)
                {
                    const ReportHandler& report = reports[i];
                    const std::unordered_map<std::string, double>& reportPrices = report.GetPrices();
                    double price = reportPrices.find(categoryName)->second;
                    add(em::utils::FormatDoubleToString(price));

                    totalPrices[i] += price;
                }
                endOfRow();
            }

            // add an empty row before printing total
            add("").add("").add("").endOfRow();

            // add the total row
            add("TOTAL");
            for (double total : totalPrices)
                add(em::utils::FormatDoubleToString(total));
            endOfRow();
        }

        void AddHeader(const std::vector<ReportHandler>& reports)
        {
            add("Category");
            for (const ReportHandler& report : reports)
                add(utils::date::GetMonthNameFromNumber(report.GetUnit()));
            endOfRow();
        }
    };

    class TextTable_Account : public TextTable
    {
    public:
        TextTable_Account(const std::vector<db::Model>& rows)
            : TextTable()
            , m_Rows(rows)
        {
            add("ROW_ID").add("NAME").add("DESCRIPTION").endOfRow();
            for (const db::Model& row : m_Rows)
            {
                add(std::to_string(row.at("row_id").asInt()))
                    .add(row.at("name").asString())
                    .add(row.at("description").asString())
                    .endOfRow();
            }
        }

    private:
        const std::vector<db::Model>& m_Rows;
    };

}
