//
// Created by ricardohb on 13/11/18.
//

#include "csv_row.h"
#include <sstream>

std::string const& csv_row::operator[](std::size_t index) const
{
    return m_data[index];
}
std::size_t csv_row::size() const
{
    return m_data.size();
}
void csv_row::read_next_row(std::istream& str)
{
    std::string         line;
    std::getline(str, line);

    std::stringstream   line_stream(line);
    std::string         cell;

    m_data.clear();
    while(std::getline(line_stream, cell, ','))
    {
        m_data.push_back(cell);
    }
    // This checks for a trailing comma with no data after it.
    if (!line_stream && cell.empty())
    {
        // If there was a trailing comma then add an empty element.
        m_data.emplace_back("");
    }
}
