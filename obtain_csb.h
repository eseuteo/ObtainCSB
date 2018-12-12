//
// Created by ricardohb on 13/11/18.
//

#ifndef OBTAIN_CSB_OBTAIN_CSB_H
#define OBTAIN_CSB_OBTAIN_CSB_H

#include <iostream>
#include "csv_row.h"
#include <sstream>

#define ROW true
#define COLUMN false
#define HIT_SCORE 4
#define GAP_PENALTY 1
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
bool operator!(std::ifstream& str);
std::istream& operator>>(std::istream& str, csv_row& data);
double get_tilt(uint64_t x1, uint64_t x2, uint64_t y1, uint64_t y2);
int64_t get_score(uint64_t a, uint64_t b);
std::string get_last_word(const std::string& s);
uint8_t get_case_max(int64_t diagonal, int64_t left, int64_t up);
std::tuple<uint64_t, uint64_t> get_sequence_lengths(std::istream & file);
int64_t get_row_max_score(int64_t ** score_matrix, uint64_t i, uint64_t j);
int64_t get_col_max_score(int64_t ** score_matrix, uint64_t i, uint64_t j);
int64_t obtain_row_max_score(double ratio, int64_t current_max_row_score, int64_t left_cell_score);
int64_t obtain_column_max_score(double ratio, int64_t column_max_score, int64_t above_cell_score);
int64_t obtain_score_matrix(double ratio, int64_t center, int64_t left, int64_t above);
#endif //OBTAIN_CSB_OBTAIN_CSB_H