//
// Created by ricardohb on 4/12/18.
//

#ifndef OBTAIN_CSB_SCORE_MATRIX_CELL_H
#define OBTAIN_CSB_SCORE_MATRIX_CELL_H


#include <cstdint>

class score_matrix_cell {
public:
    uint64_t i;
    uint64_t j;
    score_matrix_cell();
    score_matrix_cell(uint64_t i, uint64_t j);

};


#endif //OBTAIN_CSB_SCORE_MATRIX_CELL_H
