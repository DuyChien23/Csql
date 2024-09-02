//
// Created by chiendd on 21/07/2024.
//

#ifndef ERRORS_H
#define ERRORS_H
#include <sstream>

namespace Csql {
    enum ErrorCode {
        // operation errors
        tableExistedError = 1050,
        tableNotExistedError = 1051,
        tableReferenceNotFoundError = 1060,
        tooManyPrimaryKeysError = 1061,
        keyNotNullError = 1071,
        expressionError = 1080,

        // storage i/o related
        readError = 9000,
        writeError = 9010,

        // unknown error. 10000
        unknownError = 10000,
    };

    class Errors : public std::exception {
    public:
        explicit Errors(ErrorCode errorCode) : code(errorCode) {};
        explicit Errors(ErrorCode errorCode, std::string msg) : code(errorCode), msg(msg) {};

        const char* what() const noexcept override {
            switch (code) {
                // operation errors
                case tableExistedError:             return "table existed";
                case tableNotExistedError:          return "table not existed";
                case tableReferenceNotFoundError:   return "table reference not found";
                case tooManyPrimaryKeysError:       return "too many primany keys";
                case keyNotNullError:               return ("key " + msg + " is not null").c_str();
                case expressionError:               return "expression got error";

                // storage i/o related
                case readError:                     return "read error";
                case writeError:                    return "write error";

                // unknown error. 10000
                default:                            return "unknown error";
            }
        }
    protected:
        ErrorCode code;
        std::string msg;
    };
}

#endif //ERRORS_H
