#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

namespace torq {

    enum TokenType {
        LPAREN,
        RPAREN,
        NL,
        EOS,
        ERROR
    };

    class Token {
        public:
        TokenType type;
        int line;
        int column;
        std::string value;

        Token(TokenType type, int line, int column, std::string value) :
            type(type), line(line), column(column), value(value) {}
    };



    class Lexer {
        private:
        std::istream *source;

        std::stringstream string_stream;

        public:
        Lexer(std::string string_source) : string_stream(string_source) {source = &string_stream;};
        Lexer(std::ifstream file_source) {source = &file_source;};

        Token next();
        Token peek();
    };

}
