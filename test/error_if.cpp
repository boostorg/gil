#include <exception>

void error_if(bool condition) {
    if (condition)
        throw std::exception();
}

