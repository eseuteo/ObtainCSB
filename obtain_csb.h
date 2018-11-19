//
// Created by ricardohb on 13/11/18.
//

#ifndef OBTAIN_CSB_OBTAIN_CSB_H
#define OBTAIN_CSB_OBTAIN_CSB_H

#include <iostream>
#include "csv_row.h"

#define ROW true
#define COLUMN false
#define GAP_PENALTY 3
#define TOP_SCORE_SIZE 50
#define DIAGONAL 10
#define LEFT 11
#define UP 12

struct frag_info {
    uint64_t x_start;
    uint64_t y_start;
    uint64_t frag_length;
    uint64_t frag_id;
    char strand;
};

struct score_cell {
    int64_t score;
    uint64_t i;
    uint64_t j;
    score_cell *next;
};

bool is_near(struct score_cell *a, struct score_cell *b);
bool operator==(const struct score_cell& lhs, const struct score_cell& rhs);
std::istream& operator>>(std::istream& str, csv_row& data);
double get_tilt(uint64_t x1, uint64_t x2, uint64_t y1, uint64_t y2);

#endif //OBTAIN_CSB_OBTAIN_CSB_H