//
// Created by ricardohb on 13/11/18.
//

#ifndef OBTAIN_CSB_CSV_ROW_H
#define OBTAIN_CSB_CSV_ROW_H

#include <iostream>
#include <vector>

class csv_row {
    public:
        std::string const& operator[](std::size_t index) const;
        std::size_t size() const;
        void read_next_row(std::istream& str);
    private:
        std::vector<std::string>    m_data;
};


#endif //OBTAIN_CSB_CSV_ROW_H