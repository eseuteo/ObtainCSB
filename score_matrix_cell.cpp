//
// Created by ricardohb on 4/12/18.
//

#include "score_matrix_cell.h"


score_matrix_cell:: score_matrix_cell () {
    this->i = 0;
    this->j = 0;
}

score_matrix_cell:: score_matrix_cell (uint64_t i, uint64_t j) {
    this->i = i;
    this->j = j;
}