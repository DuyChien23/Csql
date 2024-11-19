//
// Created by chiendd on 24/09/2024.
//

#ifndef SCANER_H
#define SCANER_H
#include <functional>
#include <istream>

using TerminalChecking = std::function<bool(char)>;;

class Scanner {
public:
    explicit Scanner(std::istream &anInput) : input(anInput) {
    }

    [[nodiscard]] bool eof() const { return input.eof(); }

    [[nodiscard]] std::string read(const TerminalChecking &terminalChecking) const {
        std::string theResult;
        while (!eof() && terminalChecking(input.peek())) {
            char theChar = input.get();
            theResult += theChar;
        }
        return theResult;
    }

    [[nodiscard]] std::string readOne() const {
        std::string theResult;
        theResult += input.get();
        return theResult;
    }

    [[nodiscard]] std::string readUntilChar(const char &theChar) const {
        return read([&](char aChar) {
            return theChar != aChar;
        });
    }

    void skip(const TerminalChecking &terminalChecking) const {
        while (!eof() && terminalChecking(input.peek())) {
            input.get();
        }
    }

    [[nodiscard]] bool skipIfChar(char theChar) const {
        if (eof() || input.peek() != theChar) return false;
        input.get();
        return true;
    }

protected:
    std::istream &input;
};


#endif //SCANER_H
