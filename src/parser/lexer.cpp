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

    char Lexer::peek_char(int offset) {

        if( offset == 1) {
            return source->peek();
        } else {
            //store current position
            int curr = source->tellg();

            // this looks weird, but is ok.
            // istream->peek looks at the character after the current position (curr + 1).
            // So peek_char(1) - peek 1 char ahead - needs an offset of 0
            source->seekg(offset-1, source->cur);
            char ch = source->peek();

            //reset to original pos
            source->seekg(curr, source->beg);

            return ch;
        }
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
                    //while( (peek_char(1) != '\n') && (!source->eof()) ) {
                    while(true) {
                        ch = peek_char();

                        if(ch == '\n') {
                            //skip over \n
                            advance();
                            break;
                        } else if (ch == std::istream::traits_type::eof() ) {
                            //end of file - jump out
                            break;
                        } else {
                            //next char
                            advance();
                        }
                    }
                    //set tmp to the next char - might be and eof
                    tmp = advance();
                    break;

                default:
                    return tmp;
            }
        }
    }

    bool Lexer::is_hex_char(char ch) {
        if ( (ch >= '0') && (ch <= '9') )
            return true;
        else if ( (ch >= 'a') && (ch <= 'f') )
            return true;
        else if ( (ch >= 'A') && (ch <= 'F') )
            return true;
        else if (ch =='_')
            return true;

        return false;
    }

    Token Lexer::read_hex_number() {
        std::string buffer = "";
        int start_column = column;

        while(true){
            if( is_hex_char(peek_char()) ) {
                char ch = advance();
                if(ch != '_') {
                    buffer += ch;
                }
            } else {
                //try to convert to long
                try {
                    long value = std::stol(buffer, nullptr, 16);
                    return Token(INTEGER, line, start_column, value);
                } catch (const std::exception& e){
                    return Token(ERROR, line, start_column, "Unable to convert hex literal to integer");
                }
            }
        }
    }

    bool Lexer::is_binary_char(char ch) {
        if ( (ch >= '0') && (ch <= '1') )
            return true;
        else if (ch =='_')
            return true;

        return false;
    }

    Token Lexer::read_binary_number() {
        std::string buffer = "";
        int start_column = column;

        while(true){
            if( is_binary_char(peek_char()) ) {
                char ch = advance();
                if(ch != '_') {
                    buffer += ch;
                }
            } else {
                //try to convert to long
                try {
                    long value = std::stol(buffer, nullptr, 2);
                    return Token(INTEGER, line, start_column, value);
                } catch (const std::exception& e){
                    return Token(ERROR, line, start_column, "Unable to convert binary literal to integer");
                }
            }
        }
    }

    bool Lexer::is_decimal_char(char ch) {
        if ( (ch >= '0') && (ch <= '9') )
            return true;
        else if (ch =='_')
            return true;

        return false;
    }
    Token Lexer::read_number() {
        std::string buffer = "";
        int start_column = column;

        bool decimal = true;

        while(true) {
            char ch = peek_char(1);

            if( is_decimal_char(ch) ) {
                ch = advance();
                if(ch != '_') {
                    buffer += ch;
                }

            } else if( (ch == '.') && decimal ){
                ch = advance();
                decimal = false;
                buffer += ch;

            } else if( (ch == 'e') || (ch == 'E') ) {
                advance();

                //look for an exponent sign after the exponent marker
                char sch = peek_char(1);

                if( (sch == '-') || (sch == '+') ) {
                    //look for a digit after the exponent sign
                    char pch = peek_char(2);

                    if(is_decimal_char(pch)) {
                        decimal = false;

                        buffer += ch;
                        buffer += sch;
                        buffer += pch;
                        advance();
                        advance();
                    } else {
                        return Token(ERROR, line, start_column, "incomplete float literal");
                    }

                } else if(is_decimal_char(sch)) {
                    decimal = false;

                    buffer += ch;
                    buffer += sch;
                    advance();

                } else {
                    return Token(ERROR, line, start_column, "incomplete float literal");
                }
            } else {
                break;
            }
        }

        if(decimal) {
            try {
                long value = std::stol(buffer, nullptr, 10);
                return Token(INTEGER, line, start_column, value);
            } catch (const std::exception& e) {
                return Token(ERROR, line, start_column, "Error converting decimal number literal");
            }
        } else {
            try{
            double value = std::stod(buffer);
            return Token(FLOAT, line, start_column, value);
            } catch(const std::exception& e) {
                return Token(ERROR, line, start_column, "Error converting float number literal");
            }
        }
    }

    Token Lexer::read_string() {
        std::string buffer = "";
        bool single_line = true;
        int start_column = column-1;
        int start_line = line;

        //check for single or multi-line strings and for empty strings
        if( (peek_char(1) == '"') && (peek_char(2) != '"') ) {
            //empty string
            advance();
            return Token(STRING, line, start_column, "");
        } else if ( (peek_char(1) == '"') && (peek_char(2) == '"') ) {
            //multi-line string
            single_line = false;
            advance();
            advance();
        } else {
            single_line = true;
        }

        while(true) {
            char ch = advance();

            if(ch == '\\') {
                ch = advance();

                switch(ch) {
                    case '\\': buffer += '\\'; break;
                    case '"': buffer += '"'; break;
                    case 'n': buffer += '\n'; break;
                    case 'r': buffer += '\r'; break;
                    case 't': buffer += '\t'; break;

                    default:
                        buffer = "Invalid escape character: ";
                        buffer += ch;
                        return Token(ERROR, line, column, buffer);
                }
            } else if(ch == '\n') {
                if(single_line) {
                    advance();
                    line++;
                    return Token(ERROR, line, column, "Unterminated string literal");
                } else {
                    //multiline, just add
                    buffer += ch;
                }
            } else if(ch == '"') {
                if(single_line) {
                    return Token(STRING, line, start_column, buffer);
                } else {
                    //multiline string - read and remove two more " or error
                    if( (peek_char(1) == '"') && (peek_char(2) == '"') ) {
                        return Token(STRING, start_line, start_column, buffer);
                    } else {
                        return Token(ERROR, start_line, start_column, "unclosed multi-line string");
                    }
                }
            } else if(source->eof()) {
                return Token(ERROR, line, column, "Unterminated string literal");
            } else {
                buffer += ch;
            }
        }
    }

    Token Lexer::process_pair(char second, TokenType pair, TokenType single) {
        if (peek_char() == second) {
            advance();
            return Token(pair, line, column-1, "");
        } else {
            return Token(single, line, column, "");
        }
    }

    Token Lexer::read_token() {
        char ch = advance();

        //skip whitespace and comments - in any order - before collecting next token
        ch = skip_whitespace_comments(ch);

        if (ch == std::istream::traits_type::eof() )
            return Token(EOS, line, column, "");

        //assign tokens
        switch(ch) {
            //single char tokens
            case '(': return Token(LPAREN, line, column);
            case ')': return Token(RPAREN, line, column);
            case '[': return Token(LBRACKET, line, column);
            case ']': return Token(RBRACKET, line, column);
            case ',':  return Token(COMMA, line, column);
            case '.': return Token(DOT, line, column);
            case ';': return Token(SEMICOLON, line, column);
            case '+': return Token(PLUS, line, column);
            case '-': return Token(MINUS, line, column);
            case '*': return Token(STAR, line, column);
            case '/': return Token(SLASH, line, column);
            case '%': return Token(PERCENT, line, column);
            case '\n':
                line++;
                return Token(ENDL, line, column);

            //single or double char tokens
            case '=': return process_pair('=', EQUALS, ASSIGN);
            case '>': return process_pair('=', GTE, GT);
            case '<': return process_pair('=', LTE, LT);
            case '!': return process_pair('=', NOTEQUALS, EXCLAIM);

            //multi-char tokens

            // hex and binary formatted numbers
            // floats and decimals starting with 0
            // rest below
            case '0':
                if(peek_char() == 'x') {
                    advance();
                    return read_hex_number();
                } else if (peek_char() == 'b') {
                    advance();
                    return read_binary_number();
                } else {
                    return read_number();
                }

            //single and multi-line strings
            case '"': return read_string();

            default:
                if( (ch >= '0') && (ch <= '9') ) {
                    //rewind 1 place to allow the read routine to pick up the first digit
                    source->seekg(-1, source->cur);
                    return read_number();
                } else {
                    std::string error = "unrecognised token: ";
                    error += ch;
                    return Token(ERROR, line, column, error);
                }
        }
    }
}
