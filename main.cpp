#include <iostream>
#include "common_functions.h"
#include "obtain_csb.h"
#include "csv_row.h"
#include "score_list.h"
#include <tuple>
#include <sstream>
#include <limits>
#include <cmath>

int8_t get_score(uint64_t a, uint64_t b) {
    if (a && a == b) {
        // std::cout << "Hit: " << a << ", " << b << std::endl;
        return HIT_SCORE;
    }
    else
        return -HIT_SCORE;
}

bool could_not_open(std::ifstream & file) {
    return !file;
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

std::tuple<uint64_t, uint64_t> get_sequence_lengths(std::ifstream & file) {
    uint64_t x_length, y_length;
    uint8_t line = 0;
    std::string length_string;
    while (std::getline(file, length_string)){
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

int main(int argc, char *argv[]) {
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

    std::ifstream header_file;

    header_file.open(input_header_file_name);

    if (could_not_open(header_file)) {
        terror("Could not open header file");
    }

    std::tuple<uint64_t, uint64_t> sequence_lengths = get_sequence_lengths(header_file);
    header_file.close();

    uint64_t x_sequence_length = std::get<0>(sequence_lengths);
    uint64_t y_sequence_length = std::get<1>(sequence_lengths);

    std::ifstream frags_file;
    frags_file.open(input_frags_file_name);
    csv_row row;

    uint64_t cell_size = std::numeric_limits<uint64_t>::max();
    std::cout << cell_size << std::endl;

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

    uint64_t x_size = 0;
    uint64_t y_size = 0;
    x_size = x_sequence_length/cell_size + 1;
    y_size = y_sequence_length/cell_size + 1;

    double x_y_ratio = (double) x_size/y_size;
    std::cout << "[RATIO]: " << x_y_ratio << std::endl;

    std::cout << x_size << ", " << y_size << std::endl;
    uint64_t *x_sequence = new uint64_t[x_size];
    uint64_t *y_sequence = new uint64_t[y_size];

    std::vector<struct frag_info>::iterator it = frag_info_vector.begin();
    uint64_t cells_to_fill;
    uint64_t start, end;
    for (it; it != frag_info_vector.end(); ++it) {
        cells_to_fill = (*it).frag_length / cell_size;
        start = (*it).x_start / cell_size;
        end = start + cells_to_fill;
        for (start; start < end; start++) {
            x_sequence[start + 1] =  (*it).frag_id;
        }
        start = (*it).y_start / cell_size;
        end = start + cells_to_fill;
        for (start; start < end; start++) {
            y_sequence[start + 1] =  (*it).frag_id;
        }
    }

    // for (int64_t i = 0; i < std::min(x_size, y_size); i++) {
    //     std::cout << x_sequence[i] << "," << y_sequence[i] << std::endl;
    // }

    int64_t above_cell_score;
    int64_t left_cell_score;
    int64_t diagonal_cell_score;

    int64_t current_cell_score;
    int64_t row_max_score = 0;
    int64_t *column_max_score = new int64_t[y_size];

    int64_t **score_matrix = new int64_t *[x_size];
    for (int64_t i = 0; i < x_size; i++) {
        score_matrix[i] = new int64_t[y_size];
    }

    for (uint64_t i = 0; i < x_size; i++) {
        for (uint64_t j = 0; j < y_size; j++) {
            score_matrix[i][j] = 0;
        }
    }

    score_list list_top_scores = score_list();
    uint64_t size_list;

    std::ofstream output_file_score("score_matrix.csv");
    std::cout << "Filling score matrix" << std::endl;
    for (uint64_t i = 1; i < x_size; i++) {
        if (i % (x_size / 10) == 0) {
            std::cout << i*100/x_size << std::endl;
        }
        for (uint64_t j = 1; j < y_size; j++) {
            score_matrix[i][j] = 0;

//            if (x_sequence[i] && x_sequence[i] == y_sequence[j]) {
//                output_file_score << "[" << x_sequence[i] * cell_size << ", " << y_sequence[j] * cell_size << "]: " << current_cell_score << std::endl;
//                // output_file_score << "[" << i * cell_size << ", " << j * cell_size << "]: " << current_cell_score << std::endl;
//            }
            current_cell_score = get_score(x_sequence[i], y_sequence[j]) + score_matrix[i-1][j-1];
//            row_max_score = std::max((int64_t) 0, std::max(row_max_score - (int64_t) GAP_PENALTY, score_matrix[i][j-1] - (int64_t) GAP_PENALTY)); // No guardar el score sino la coordenada?

            row_max_score = obtain_row_max_score(x_y_ratio, row_max_score, score_matrix[i][j-1]);
            //std::cout << "row " << row_max_score << std::endl;
            column_max_score[j] = obtain_column_max_score(x_y_ratio, column_max_score[j], score_matrix[i-1][j]);
            //std::cout << "col " << column_max_score[j] << std::endl;

            score_matrix[i][j] = obtain_score_matrix(x_y_ratio, current_cell_score, row_max_score, column_max_score[j]);

            if (score_matrix[i][j] >= 94515711529264) {
                std::cout << "ERROR: " << i << ", " << j << ", " << current_cell_score << ", " << row_max_score << ", " << column_max_score[j] << std::endl;
                exit(-1);
            }
            if (score_matrix[i][j] > 8) {
                size_list = list_top_scores.length;
                list_top_scores.insert(score_matrix[i][j], i, j);
                list_top_scores.remove_near_scores();
//                if (size_list < list_top_scores.length) {
//                    std::cout << "Inserted: " << std::endl;
//                    std::cout << "[" << i << ", " << j << ", " << score_matrix[i][j] << "]"<< std::endl;
//                    std::cout << "Pre: \n" << std::endl;
//                    list_top_scores.print_list();
//                    std::cout << "\n" << std::endl;
//                    std::cout << "Length: " << list_top_scores.length << std::endl;
//                   list_top_scores.remove_near_scores();
//                    std::cout << "Post: \n" << std::endl;
//                    list_top_scores.print_list();
//                    std::cout << "\n" << std::endl;
//                    std::cout << "Lenght: " << list_top_scores.length << std::endl;
//                }
            }
        }
        // output_file_score << '\n';
    }

    list_top_scores.print_list();
    uint64_t x_start, y_start, x_end, y_end;
    uint64_t starting_index_i, starting_index_j, ending_index_i, ending_index_j;
    uint8_t case_max;
    bool stop;
    std::ofstream output_file(output_file_name);
    uint64_t len = list_top_scores.length;
    std::cout << len << std::endl;
    for (uint64_t i = 0; i < len; i++) {
        stop = false;
        struct score_cell current_max = list_top_scores.get_last();
        starting_index_i = ending_index_i = current_max.i;
        starting_index_j = ending_index_j = current_max.j;
        while (!stop) {
            case_max = get_case_max(score_matrix[ending_index_i-1][ending_index_j-1], score_matrix[ending_index_i][ending_index_j-1], score_matrix[ending_index_i-1][ending_index_j]);
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
            stop = score_matrix[ending_index_i][ending_index_j] == 0;
        }

        double tilt = get_tilt(ending_index_i, starting_index_i, ending_index_j, starting_index_j);

        if (abs(get_tilt(ending_index_i, starting_index_i, ending_index_j, starting_index_j) - M_PI/4) < 0.15) {
            output_file << ending_index_i * cell_size << ",";
            output_file << ending_index_j * cell_size << ",";
            output_file << starting_index_i * cell_size << ",";
            output_file << starting_index_j * cell_size << ",";
            output_file << tilt << ",";
            output_file << "\n";
        }
        // list_top_scores.print_list();
        list_top_scores.remove_last();
        // list_top_scores.print_list();
    }
}