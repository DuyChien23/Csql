//
// Created by chiendd on 22/07/2024.
//

#include <iostream>

#include "core/view/table_view.h"

int main() {
     TableView table_view("abc", "def");
     table_view.show(std::cerr);
}
