//
// Created by chiendd on 22/07/2024.
//

#include <fstream>
#include <iostream>

#include "core/controller/tokenizer/tokenizer.h"


int main() {
     std::fstream inp("abc.txt", std::fstream::in);
     Csql::Tokenizer tokenizer(inp);
     tokenizer.tokenize();
     for (int i = 0; i < tokenizer.size(); ++i) {
         auto x = tokenizer.tokenAt(i);
         std::cout << x->data << '\n';
     }
}
