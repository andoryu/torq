#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

namespace torq {

    enum TokenType {
        LPAREN,
        RPAREN,
        DOT,
        ASSIGN,
        GT,
        GTE,
        LT,
        LTE,
        EQUALS,
        ENDL,
        EOS = 1000,
        ERROR = 9999
    };


    class Token {
      public:
        TokenType type;
        int line;
        int column;
        std::string s_value;
        long i_value;
        double f_value;

      public:
        Token(TokenType type, int line, int column, std::string value) :
            type(type), line(line), column(column), s_value(value) {}

        Token(TokenType type, int line, int column, long value) :
            type(type), line(line), column(column), i_value(value) {}

        Token(TokenType type, int line, int column, double value) :
            type(type), line(line), column(column), f_value(value) {}
    };


    class Lexer {
      private:
        std::istream *source;
        std::stringstream string_stream;

        int line;
        int column;

        char advance();
        char skip_whitespace_comments(char ch);

      public:
        Lexer(std::string string_source) : string_stream(string_source) {
            source = &string_stream;
            line = 1;
            column = 0;
        };
        Lexer(std::ifstream &file_source) {
            source = &file_source;
            line = 1;
            column = 0;
        };

        Token next();
        Token peek();


      private:
        Token read_token();
    };

}
