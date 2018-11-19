//
// Created by ricardohb on 13/11/18.
//


#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cmath>

/*
  Print the error message and exit(-1)
*/
void terror(const std::string s) {
    std::cout << "Error: " << s << std::endl;
    exit(-1);
}

bool cannot_open_file(std::ifstream file){
    return !file;
}

double get_tilt(uint64_t x1, uint64_t x2, uint64_t y1, uint64_t y2) {
    return atan((double) (y2 - y1)/(x2 - x1));
}

