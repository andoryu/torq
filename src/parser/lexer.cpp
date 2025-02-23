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

    char Lexer::advance() {
        char ch = source->get();
        column++;
        return ch;
    }

    char Lexer::skip_whitespace_comments(char ch) {
        char tmp = ch;

        while(true) {
            switch(tmp) {
                //whitespace
                case ' ':
                case '\f':
                case '\t':
                    tmp = advance();
                    break;

                //comments till end of line
                case '#':
                    //run to end of line, or end of file
                    while( (source->peek() != '\n') && (!source->eof()) ) {
                        advance();
                    }
                    //skip the \n
                    advance();
                    //set tmp to the value after the new line
                    tmp = advance();
                    break;

                default:
                    return tmp;
            }
        }
    }

    Token Lexer::read_token() {
        char ch = advance();

        //skip whitespace and comments - in any order - before collecting next token
        ch = skip_whitespace_comments(ch);

        if (source->eof())
            return Token(EOS, line, column, "");

        //assign tokens
        switch(ch) {
            //single char tokens
            case '(': return Token(LPAREN, line, column, "");
            case ')': return Token(RPAREN, line, column, "");
            case '.': return Token(DOT, line, column, "");
            case '\n':
                line++;
                return Token(ENDL, line, column, "");

            //single or double char tokens
            case '=':
                if (source->peek() == '=') {
                    advance();
                    return Token(EQUALS, line, column-1, "");
                } else {
                    return Token(ASSIGN, line, column, "");
                }

            case '>':
                if (source->peek() == '=') {
                    advance();
                    return Token(GTE, line, column-1, "");
                } else {
                    return Token(GT, line, column, "");
                }

            //multi-char tokens

            default:
                auto s = std::string(1, ch);
                return Token(ERROR, line, column, s);
        }
    }
}
