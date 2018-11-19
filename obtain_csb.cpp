//
// Created by ricardohb on 13/11/18.
//

#include "obtain_csb.h"

std::istream& operator>>(std::istream& str, csv_row& data) {
    data.read_next_row(str);
    return str;
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