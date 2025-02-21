#include "lexer.hpp"

// #include <iostream>

namespace torq {

    Token Lexer::next() {
        return Token(ERROR, 0, 0, "");
    }

    Token Lexer::peek(){
        return Token(ERROR, 0, 0, "");
    }
}
