#ifndef HELPERS_H
#define HELPERS_H

#include <string>

using namespace std;

bool validateInput(string input) {
    for (size_t i = 0; i < input.length(); i++) {
        if (isalpha(input[i])) {
            return false;
        }
    }

    return true;
}

bool isNumber(string input) {
    for (size_t i = 0; i < input.length(); i++) {
        if (!isdigit(input[i])) {
            return false;
        }
    }

    return true;
}

#endif