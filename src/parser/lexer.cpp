#include "lexer.hpp"

#include <iostream>

namespace torq {

    Token Lexer::next() {
        return read_token();
    }


    Token Lexer::peek() {
        //save current positions and reset after
        auto current_stream_pos = source->tellg();
        int current_column = column;

        Token t = read_token();

        source->clear(); //need to clear flags, in case we hit EOF while getting the next token
        source->seekg(current_stream_pos);
        column = current_column;

        return t;
    }

    Token Lexer::read_token() {

        char ch = source->get();
        column++;

        if (source->eof())
            return Token(EOS, line, column, "");

        switch(ch) {
            case '(': return Token(LPAREN, line, column, "");
            case ')': return Token(RPAREN, line, column, "");
            case '\n': return Token(NL, line, column, "");

            default:
                auto s = std::string(1, ch);
                return Token(ERROR, line, column, s);
        }
    }
}
