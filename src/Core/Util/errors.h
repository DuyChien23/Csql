//
// Created by chiendd on 21/07/2024.
//

#ifndef ERRORS_H
#define ERRORS_H

namespace Csql {
    enum Errors {
        // storage i/o related. Range: 7000-7999
        readError = 7000,
        writeError = 7010,

        // unknown error. 10000
        unknownError = 10000
    };
}

#endif //ERRORS_H
