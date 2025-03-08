#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>

namespace torq {

    enum TokenType {
        LPAREN, RPAREN, LBRACKET, RBRACKET,
        DOT, COMMA, SEMICOLON, COLON, EXCLAIM,

        ASSIGN,

        GT, GTE, LT, LTE, EQUALS, NOTEQUALS,

        PLUS, MINUS, STAR, SLASH, PERCENT,

        INTEGER_LIT, FLOAT_LIT, STRING_LIT,

        IDENTIFIER,

        IMPORT,
        IF, THEN, ELSE, END, WHILE, FOR, DO, BREAK, RETURN,
        TRUE_LIT, FALSE_LIT,
        INT_TYPE, FLOAT_TYPE, STRING_TYPE,
        FUNCTION,

        ENDL,
        EOS = 1000,
        ERROR = 9999
    };


    inline std::unordered_map<std::string, TokenType> keywords = {
        {"import", IMPORT},
        {"if", IF},
        {"then", THEN},
        {"else", ELSE},
        {"end", END},
        {"while", WHILE},
        {"for", FOR},
        {"do", DO},
        {"break", BREAK},
        {"return", RETURN},
        {"true", TRUE_LIT},
        {"false", FALSE_LIT},
        {"int", INT_TYPE},
        {"float", FLOAT_TYPE},
        {"string", STRING_TYPE},
        {"fn", FUNCTION}
    } ;


    class Token {
      public:
        TokenType type;
        int line;
        int column;

        std::string s_value;
        long i_value;
        double f_value;

      public:
        Token(TokenType type, int line, int column) :
          type(type), line(line), column(column) {
              s_value= "";
              i_value = 0;
              f_value = 0.0f;
          }

        Token(TokenType type, int line, int column, std::string value) :
            type(type), line(line), column(column), s_value(value) {
                i_value = 0;
                f_value = 0.0f;
            }

        Token(TokenType type, int line, int column, long value) :
            type(type), line(line), column(column), i_value(value) {
                s_value= "";
                f_value = 0.0f;
            }

        Token(TokenType type, int line, int column, double value) :
            type(type), line(line), column(column), f_value(value) {
                s_value= "";
                i_value = 0;
            }
    };


    class Lexer {
      private:
        std::istream *source;
        std::stringstream string_stream;

        int line;
        int column;

        char advance();
        char peek_char(int offset = 1);

        char skip_whitespace_comments(char ch);

        Token process_pair(char second, TokenType pair, TokenType single);

        Token read_hex_number();
        Token read_binary_number();
        Token read_number();
        Token read_string();
        Token read_name();

        bool is_hex_char(char ch);
        bool is_binary_char(char ch);
        bool is_decimal_char(char ch);
        bool is_name_start_char(char ch);
        bool is_name_char(char ch);

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
