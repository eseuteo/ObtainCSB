#include <iostream>
#include "common_functions.h"
#include "obtain_csb.h"
#include "csv_row.h"
#include "score_list.h"
#include <tuple>
#include <sstream>
#include <limits>
#include <cmath>
#include <unordered_map>
#include <sparsehash/dense_hash_map>
#include <bits/unordered_map.h>
#include "score_matrix_cell.h"

struct eq_uint64_t {
    bool operator()(const uint64_t u1, const uint64_t u2) const {
        return u1 == u2;
    }
};

uint64_t get_elem(std::unordered_map<score_matrix_cell, uint64_t> map, uint64_t index_i, uint64_t index_j) {
    uint64_t ans;
    score_matrix_cell index = score_matrix_cell(index_i, index_j);
    if (map.find(index) != map.end()) {
        ans = map.at(index);
    } else {
        ans = 0;
    }
    return ans;
}

uint64_t get_elem(std::unordered_map<uint64_t, uint64_t> map, uint64_t index) {
    uint64_t ans;
    if (map.find(index) != map.end()) {
        ans = map.at(index);
    } else {
        ans = 0;
    }
    return ans;
}

int main(int argc, char *argv[]) {
    // Files
    std::string input_header_file_name;
    std::string input_frags_file_name;
    std::string output_file_name;

    if (argc != 4){
        // TODO: help, argument error messages
        terror("Incorrect number of arguments");
    }

    input_header_file_name = argv[1];
    input_frags_file_name = argv[2];
    output_file_name = argv[3];


    // Header file opening and reading
    std::ifstream header_file;

    header_file.open(input_header_file_name);

    if (!header_file.is_open()) {
        terror("Could not open header file");
    }

    std::tuple<uint64_t, uint64_t> sequence_lengths = get_sequence_lengths(header_file);
    header_file.close();

    uint64_t x_sequence_length = std::get<0>(sequence_lengths);
    uint64_t y_sequence_length = std::get<1>(sequence_lengths);

    // Frags file opening and reading
    std::ifstream frags_file;
    frags_file.open(input_frags_file_name);
    csv_row row;

    uint64_t cell_size = std::numeric_limits<uint64_t>::max();

#ifdef VERBOSE
    std::cout << cell_size << std::endl;
#endif

    struct frag_info current_frag;
    std::vector<struct frag_info> frag_info_vector;
    uint64_t frag_id = 0;
    std::string type;

    while(frags_file >> row) {
        std::istringstream iss1(row[1]);
        iss1 >> current_frag.x_start;
        std::istringstream iss2(row[2]);
        iss2 >> current_frag.y_start;
        std::istringstream iss7(row[7]);
        iss7 >> current_frag.frag_length;
        current_frag.strand = row[5].find('f') == -1 ? 'r' : 'f';
        current_frag.frag_id = frag_id;

        frag_info_vector.push_back(current_frag);

        if (cell_size > current_frag.frag_length && current_frag.frag_length != 0) {
            cell_size = current_frag.frag_length;
        }
        frag_id++;
    }
    frags_file.close();

    // Score matrix dimensions (Is this necessary for a sparse matrix??)
    uint64_t x_size = 0;
    uint64_t y_size = 0;
    x_size = x_sequence_length/cell_size + 1;
    y_size = y_sequence_length/cell_size + 1;

    // Ratio b/w x and y sequence
    double x_y_ratio = (double) x_size/y_size;

#ifdef VERBOSE
        std::cout << "[RATIO]: " << x_y_ratio << std::endl;
        std::cout << x_size << ", " << y_size << std::endl;
#endif

        // Replaced unordered_map with dense_hash_map ==
    google::dense_hash_map<uint64_t, uint64_t> x_sequence_map;
    x_sequence_map.set_empty_key(NULL);
    google::dense_hash_map<uint64_t, uint64_t> y_sequence_map;
    y_sequence_map.set_empty_key(NULL);

    // Determine what to do with each frag      O(n), being n the number of frags
    //      - How many cells are being filled
    //      - Starting and ending coordinates in x and y sequences
    std::vector<struct frag_info>::iterator it = frag_info_vector.begin();
    uint64_t cells_to_fill;
    uint64_t start, end;
    for (it; it != frag_info_vector.end(); ++it) {
        cells_to_fill = (*it).frag_length / cell_size;
        start = (*it).x_start / cell_size;
        end = start + cells_to_fill;
        for (start; start < end; start++) {
            x_sequence_map[start+1] = (*it).frag_id;
        }
        start = (*it).y_start / cell_size;
        end = start + cells_to_fill;
        for (start; start < end; start++) {
            y_sequence_map[start+1] = (*it).frag_id;
        }
    }

//#ifdef _DEBUG_SPARSE
//    uint64_t counter_sparse_x = 0;
//    for (int i = 0; i < x_size; i++) {
//        counter_sparse_x += x_sequence[i] != 0;
//    }
//    std::cout << counter_sparse_x << std::endl;
//#endif

#ifdef _DEBUG_SPARSE
    uint64_t non_zero_elements = 0;
#endif


    int64_t above_cell_score;
    int64_t left_cell_score;
    int64_t diagonal_cell_score;

    int64_t current_cell_score;
    int64_t row_max_score = 0;
    int64_t *column_max_score = new int64_t[y_size];

    // WHY DO I NEED THIS?
    uint64_t _i = 0;
    for (_i = 0; _i < y_size; _i++) {
        column_max_score[_i] = 0;
    }

    uint64_t prev_cell = 0;
    uint64_t prev_row_cell = 0;
    score_matrix_cell *aux_score_cell = new score_matrix_cell();

    uint64_t score = 0;

    std::unordered_map<score_matrix_cell, uint64_t> sparse_score_matrix;

//    int64_t **score_matrix = new int64_t *[x_size];
//    for (int64_t i = 0; i < x_size; i++) {
//        score_matrix[i] = new int64_t[y_size];
//    }

    score_list list_top_scores = score_list();
    uint64_t size_list;

    std::ofstream output_file_score("score_matrix.csv");

#ifdef VERBOSE
    std::cout << "Filling score matrix" << std::endl;
#endif

    uint64_t current_x = 0;
    uint64_t current_y = 0;

    // Score matrix filling
    // TODO: In order to change it with a sparse matrix --> get and set functions
    for (uint64_t i = 1; i < x_size; i++) {
        current_x = x_sequence_map[i];

#ifdef VERBOSE
        std::cout << i << std::endl;
        if (i % (x_size / 10) == 0 || i == 3422) {
            std::cout << i * 100 / x_size + 1 << " %" << std::endl;
        }
#endif

        for (uint64_t j = 1; j < y_size; j++) {
            //score_matrix[i][j] = 0;

#ifdef _DEBUG
            if (x_sequence[i] && x_sequence[i] == y_sequence[j]) {
                output_file_score << "[" << x_sequence[i] * cell_size << ", " << y_sequence[j] * cell_size << "]: " << current_cell_score << std::endl;
                output_file_score << "[" << i * cell_size << ", " << j * cell_size << "]: " << current_cell_score << std::endl;
            }
#endif

            current_y = y_sequence_map[j];

            // current_cell_score = get_score(current_x, current_y) + score_matrix[i-1][j-1];

            prev_cell = get_elem(sparse_score_matrix, i-1, j-1);
            current_cell_score = get_score(current_x, current_y) + prev_cell;
            // row_max_score = obtain_row_max_score(x_y_ratio, row_max_score, score_matrix[i][j-1]);
            prev_row_cell = get_elem(sparse_score_matrix, i, j-1);
            row_max_score = obtain_row_max_score(x_y_ratio, row_max_score, prev_row_cell);
            // column_max_score[j] = obtain_column_max_score(x_y_ratio, column_max_score[j], score_matrix[i-1][j]);
            column_max_score[j] = obtain_column_max_score(x_y_ratio, column_max_score[j], get_elem(sparse_score_matrix, i-1, j));

#ifdef _DEBUG
            std::cout << "row " << row_max_score << std::endl;
            std::cout << "col " << column_max_score[j] << std::endl;
#endif

            // score_matrix[i][j] = obtain_score_matrix(x_y_ratio, current_cell_score, row_max_score, column_max_score[j]);
            score = obtain_score_matrix(x_y_ratio, current_cell_score, row_max_score, column_max_score[j]);
            if (score != 0) {
                aux_score_cell = new score_matrix_cell(i, j);
                sparse_score_matrix.insert({*aux_score_cell, score});
                if (get_elem(sparse_score_matrix, i, j) > 8) {
                    size_list = list_top_scores.length;
                    list_top_scores.insert(get_elem(sparse_score_matrix, i, j), i, j);
                    list_top_scores.remove_near_scores();
                }
            }
        }
    }

#ifdef _DEBUG_SPARSE
    std::cout << "Percentage of non-zero elements: " << non_zero_elements << "/" << x_size * y_size << std::endl;
#endif

#ifdef VERBOSE
    list_top_scores.print_list();
    list_top_scores.remove_near_scores_thoroughly();
    list_top_scores.print_list();
#endif
    uint64_t x_start, y_start, x_end, y_end;
    uint64_t starting_index_i, starting_index_j, ending_index_i, ending_index_j;
    uint8_t case_max;
    bool stop;
    std::ofstream output_file(output_file_name);
    uint64_t len = list_top_scores.length;

#ifdef VERBOSE
    std::cout << len << std::endl;
#endif

    for (uint64_t i = 0; i < len; i++) {
        stop = false;
        struct score_cell current_max = list_top_scores.get_last();
        starting_index_i = ending_index_i = current_max.i;
        starting_index_j = ending_index_j = current_max.j;
        while (!stop) {
            case_max = get_case_max(get_elem(sparse_score_matrix, ending_index_i-1, ending_index_j-1),
                    get_elem(sparse_score_matrix, ending_index_i, ending_index_j-1),
                    get_elem(sparse_score_matrix, ending_index_i-1, ending_index_j));
            switch(case_max) {
                case DIAGONAL:
                    ending_index_i-=1;
                    ending_index_j-=1;
                    break;
                case LEFT:
                    ending_index_i-=1;
                    break;
                case UP:
                    ending_index_j-=1;
                    break;
            }
            stop = get_elem(sparse_score_matrix, ending_index_i, ending_index_j) == 0;
        }

        double tilt = get_tilt(ending_index_i, starting_index_i, ending_index_j, starting_index_j);

#ifdef VERBOSE
        std::cout << tilt << std::endl;
        std::cout << atan(x_y_ratio) << std::endl;

        std::cout << fabs(tilt - atan(x_y_ratio)) << std::endl;
        std::cout <<  M_PI/16 << std::endl;
#endif

        if (fabs(tilt - atan(1/x_y_ratio)) < M_PI/8) {
            output_file << ending_index_i * cell_size << ",";
            output_file << ending_index_j * cell_size << ",";
            output_file << starting_index_i * cell_size << ",";
            output_file << starting_index_j * cell_size << ",";
            output_file << tilt << ",";
            output_file << "\n";
        }
        list_top_scores.remove_last();

#ifdef VERBOSE
        list_top_scores.print_list();
#endif

    }
}