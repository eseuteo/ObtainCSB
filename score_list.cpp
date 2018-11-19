//
// Created by ricardohb on 13/11/18.
//

#include "obtain_csb.h"
#include "score_list.h"

score_list:: score_list () {
    head = nullptr;
    length = 0;
}

score_list::~score_list () {
    struct score_cell *next = head;

    while (next) {
        struct score_cell *delete_me = next;
        next = next->next;
        delete delete_me;
    }
}

uint64_t score_list::get_min_score() {
    if (head) {
        return head->score;
    }
    return 0;
}

void score_list::insert(int64_t score, uint64_t i, uint64_t j) {
    // std::cout << "Inserting" << std::endl;
    struct score_cell *new_score_cell = new score_cell{score, i, j, NULL};

    struct score_cell *next = head;
    struct score_cell *prev = NULL;
    bool found_related = is_near(new_score_cell, next);

    while (next && !found_related) {
        prev = next;
        next = next->next;
        found_related = is_near(new_score_cell, next);
    }

    if (found_related && next->score < new_score_cell->score) {
        if (prev) {
            // std::cout << "test0" << std::endl;
            // if (head) {
            //     std::cout << '[' << head->i << ", " << head->j << ", " << head->score << "]" << std::endl;
            // }
            // std::cout << "---" << std::endl;
            prev->next = next->next;
        } else {
            // std::cout << "test1" << std::endl;
            // if (head) {
            //     std::cout << '[' << head->i << ", " << head->j << ", " << head->score <<  "]" << std::endl;
            // }
            // std::cout << "+++" << std::endl;
            head = next->next;
        }
        delete(next);
        length--;
    } else if (found_related) {
        return;
    }
    next = head;
    prev = NULL;
    bool found_place = false;
    while (next && next->score < new_score_cell->score) {
        prev = next;
        next = next->next;
    }
    if (!prev) {
        new_score_cell->next = next;
        head = new_score_cell;
    } else {
        prev->next = new_score_cell;
        new_score_cell->next = next;
    }
    length++;
    if (length > TOP_SCORE_SIZE) {
        struct score_cell *delete_me = head;
        head = head->next;
        delete(delete_me);
        length--;
    }
}

struct score_cell score_list::get_last() {
    struct score_cell *last = head;
    while (last && last->next) {
        last = last->next;
    }
    return *last;
}

void score_list::remove_last() {
    // std::cout << "Removing last" << std::endl;
    struct score_cell *current = head;
    if (length == 1) {
        head = NULL;
        return;
    }
    while (current->next->next) {
        current = current->next;
    }
    score_cell *delete_me = current->next;
    current->next = NULL;
    delete delete_me;
    length--;
}

void score_list::print_list() {
    struct score_cell *next = head;

    while (next) {
        std::cout << '[' << next->i << ", " << next->j << ", " << next->score << "], ";
        next = next->next;
    }
    std::cout << std::endl;
}

void score_list::remove_near_scores() {
    struct score_cell *prev = NULL;
    struct score_cell *current = head;
    struct score_cell *next = head ? head->next : head;
    // if (length == 16) {
    //     std::cout << std::endl;
    // }

    while (next) {
        if (is_near(current, next)) {
            struct score_cell *delete_me = current;
            if (prev) {
                prev->next = current->next;
            } else {
                head = current->next;
            }
            delete(current);
            length--;
            prev = NULL;
            current = head;
            next = head ? head->next : head;
        } else {
            prev = current;
            current = next;
            next = next->next;
        }
    }
}
