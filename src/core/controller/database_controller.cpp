//
// Created by chiendd on 06/10/2024.
//

#include "database_controller.h"

namespace Csql {
    std::mutex DatabaseController::mtx;
    std::map<std::thread::id, UniqueDatabasePtr> DatabaseController::databases;

}