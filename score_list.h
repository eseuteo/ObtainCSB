//
// Created by ricardohb on 13/11/18.
//

#ifndef OBTAIN_CSB_SCORE_LIST_H
#define OBTAIN_CSB_SCORE_LIST_H

#include <cstdint>


class score_list {
    private:
        struct score_cell *head;

    public:
        uint64_t length;
        score_list();
        ~score_list();
        uint64_t get_min_score();
        void insert(int64_t score, uint64_t i, uint64_t j);
        struct score_cell get_last();
        void remove_last();
        void print_list();
        void remove_near_scores();
};


#endif //OBTAIN_CSB_SCORE_LIST_H
