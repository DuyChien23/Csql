//
// Created by chiendd on 22/07/2024.
//

#include <fstream>
#include <iostream>


void input() {
    std::fstream outfile ("test.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);
    outfile << "fuck you" << std::endl;
}
int main() {
     input();
     //output();
}
