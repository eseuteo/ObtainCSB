//
// Created by ricardohb on 4/12/18.
//

#ifndef OBTAIN_CSB_SCORE_MATRIX_CELL_H
#define OBTAIN_CSB_SCORE_MATRIX_CELL_H


#include <cstdint>
#include <functional>

class score_matrix_cell {
public:
    uint64_t i;
    uint64_t j;

    score_matrix_cell();

    score_matrix_cell(uint64_t i, uint64_t j);

    bool operator==(score_matrix_cell const &other) const;

};

namespace std {
    template<>
    struct hash<score_matrix_cell> {
        size_t operator()(score_matrix_cell const &k) const {
            return size_t((k.i * 37 + k.j) * 37);
        }
    };
}


#endif //OBTAIN_CSB_SCORE_MATRIX_CELL_H
