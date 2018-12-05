//
// Created by ricardohb on 13/11/18.
//

#include "obtain_csb.h"
#include <cmath>
#include <tuple>
#include <string>
#include <iostream>

std::istream& operator>>(std::istream& str, csv_row& data) {
    data.read_next_row(str);
    return str;
}

bool operator!(std::ifstream& str) {
    return !str;
}

bool operator==(const struct score_cell& lhs, const struct score_cell& rhs) {
    return lhs.i == rhs.i && lhs.j == rhs.j && lhs.next == rhs.next && lhs.score == rhs.score;
}


bool is_near(struct score_cell *a, struct score_cell *b) {
    if (!(a && b)) {
        return false;
    }
    int64_t i = (int64_t) a->i - (int64_t) b->i;
    int64_t j = (int64_t) a->j - (int64_t) b->j;
//    std::cout << abs(i) + abs(j) << std::endl;
//    std::cout << std::max(a->score, b->score) << std::endl;
//    std::cout << (GAP_PENALTY * (abs(i) + abs(j)) > std::max(a->score, b->score)) << std::endl;
    return GAP_PENALTY * (abs(i) + abs(j)) < std::max(a->score, b->score);
    // std::cout << "Is near?\n[" << a->i << ", " << a->j << "] -- [" << b->i << ", " << b->j << "]" << std::endl;
    //int64_t i = (int64_t) a->i - (int64_t) b->i;
    //int64_t j = (int64_t) a->j - (int64_t) b->j;
    // std::cout << (std::abs(i) < 5 && std::abs(j) < 5) << std::endl;
    //return std::abs(i) < 1000 && std::abs(j) < 1000;
}

int64_t get_score(uint64_t a, uint64_t b) {
    if (a && (a == b)) {
        return HIT_SCORE;
    }
    else
        return 0;
}

std::string get_last_word(const std::string& s) {
    auto index = s.find_last_of(' ');
    return s.substr(++index);
}

uint8_t get_case_max(int64_t diagonal, int64_t left, int64_t up){
    uint8_t ans = left > diagonal ? DIAGONAL : LEFT;
    ans = up > ans ? UP : ans;
    return ans;
}

std::tuple<uint64_t, uint64_t> get_sequence_lengths(std::istream & file) {
    uint64_t x_length, y_length;
    uint8_t line = 0;
    std::string length_string;
    while (std::getline(file, length_string)) {
        line++;
        if (line == 7) {
            std::istringstream iss(get_last_word(length_string));
            iss >> x_length;
        }
        if (line == 8) {
            std::istringstream iss(get_last_word(length_string));
            iss >> y_length;
        }
    }
    return std::make_tuple(x_length, y_length);
}

int64_t get_row_max_score(int64_t ** score_matrix, uint64_t i, uint64_t j) {
    int64_t ans = 0;
    int64_t candidate = 0;
    for (uint64_t k = 0; k < j; k++) {
        candidate = score_matrix[i][k] - (int64_t) (j - k) * (int64_t) GAP_PENALTY;
        ans = std::max(ans, candidate);
    }
    return ans;
}

int64_t get_col_max_score(int64_t ** score_matrix, uint64_t i, uint64_t j) {
    int64_t ans = 0;
    int64_t candidate = 0;
    for (uint64_t k = 0; k < i; k++) {
        candidate = score_matrix[k][j] - (int64_t) (i - k) * (int64_t) GAP_PENALTY;
        ans = std::max(ans, candidate);
    }
    return ans;
}

int64_t obtain_row_max_score(double ratio, int64_t current_max_row_score, int64_t left_cell_score) {
    int64_t ans = 0;
//    if (current_max_row_score > 10) {
//        std::cout << "Inside obtain_row_max_score: " << ratio << " " << left_cell_score << " " << current_max_row_score << std::endl;
//    }
    ans = std::max(current_max_row_score - (int64_t) GAP_PENALTY, left_cell_score - (int64_t) GAP_PENALTY);
    //std::cout << "Inside obtain_row_max_score: " << ans << std::endl;
    ans = std::max((int64_t) 0, ans);
    //std::cout << "Inside obtain_row_max_score: " << ans << std::endl;
    ans = (int64_t) (ans + ceil((GAP_PENALTY/HIT_SCORE) * ratio));
    //std::cout << "Inside obtain_row_max_score: " << ans << std::endl;
    //std::cout << "---" << std::endl;
    return ans;
}

int64_t obtain_column_max_score(double ratio, int64_t column_max_score, int64_t above_cell_score) {
    int64_t ans = 0;
    ans = std::max(column_max_score - (int64_t) GAP_PENALTY, above_cell_score - (int64_t) GAP_PENALTY);
    ans = std::max((int64_t) 0, ans);
    ans = (int64_t) (ans + ceil((GAP_PENALTY/HIT_SCORE) / ratio));
    return ans;
}

int64_t obtain_score_matrix(double ratio, int64_t center, int64_t left, int64_t above) {
    int64_t ans = center;
    if (ratio > 1) {
        ans = left;
    } else if (ratio < 1) {
        ans = above;
    }

    ans = center > ans ? center : ans;
    ans = above > ans ? above : ans;
    ans = left > ans ? left : ans;

    return ans;
}