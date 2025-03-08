#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>

#include <format>
#include <fstream>

#include "../src/parser/lexer.hpp"

int main( int argc, char* argv[] ) {
  int result = Catch::Session().run( argc, argv );
  return result;
}

TEST_CASE("extract basic characters", "[lexer]") {

    torq::Lexer l("( ) [ ] , = . ! == != > >= < <= ; + - * / % \n");
    torq::Token t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::RPAREN );

    t = l.next();
    REQUIRE( t.type == torq::LBRACKET );

    t = l.next();
    REQUIRE( t.type == torq::RBRACKET );

    t = l.next();
    REQUIRE( t.type == torq::COMMA );

    t = l.next();
    REQUIRE( t.type == torq::ASSIGN );

    t = l.next();
    REQUIRE( t.type == torq::DOT );

    t = l.next();
    REQUIRE( t.type == torq::EXCLAIM );

    t = l.next();
    REQUIRE( t.type == torq::EQUALS );

    t = l.next();
    REQUIRE( t.type == torq::NOTEQUALS );

    t = l.next();
    REQUIRE( t.type == torq::GT );

    t = l.next();
    REQUIRE( t.type == torq::GTE );

    t = l.next();
    REQUIRE( t.type == torq::LT );

    t = l.next();
    REQUIRE( t.type == torq::LTE );

    t = l.next();
    REQUIRE( t.type == torq::SEMICOLON );

    t = l.next();
    REQUIRE( t.type == torq::PLUS );

    t = l.next();
    REQUIRE( t.type == torq::MINUS );

    t = l.next();
    REQUIRE( t.type == torq::STAR );

    t = l.next();
    REQUIRE( t.type == torq::SLASH );

    t = l.next();
    REQUIRE( t.type == torq::PERCENT );

    t = l.next();
    REQUIRE( t.type == torq::ENDL );
}

TEST_CASE("return EOF token", "[lexer]") {
    torq::Lexer l("(");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::EOS );
}

TEST_CASE("handles unwanted characters", "[lexer]") {
    char ch = 0xAE; //® - registered sign
    std::string source = std::string(1, ch);
    torq::Lexer l(source);

    torq::Token t = l.next();
    REQUIRE( t.type == torq::ERROR );
}

TEST_CASE("peeks correctly", "[lexer]") {
    torq::Lexer l("()");

    torq::Token t = l.peek();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.peek();
    REQUIRE( t.type == torq::RPAREN );

    t = l.peek();
    REQUIRE( t.type == torq::RPAREN );

    t = l.peek();
    REQUIRE( t.type == torq::RPAREN );

    t = l.next();
    REQUIRE( t.type == torq::RPAREN );

    t = l.peek();
    REQUIRE( t.type == torq::EOS );

    t = l.next();
    REQUIRE( t.type == torq::EOS );
}

TEST_CASE("simple multi-line file", "[lexer]"){
    std::ifstream test_file("../tests/data/data_001.tq");

    torq::Lexer l(test_file);

    torq::Token t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::RPAREN );

    t = l.next();
    REQUIRE( t.type == torq::ENDL );

    t = l.next();
    REQUIRE( t.type == torq::RPAREN );

    t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::ENDL );

    t = l.next();
    REQUIRE( t.type == torq::EOS );
}

TEST_CASE("skips whitespace", "[lexer]"){
    torq::Lexer l("( )  \t\t   \n");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::RPAREN );

    t = l.next();
    REQUIRE( t.type == torq::ENDL );
}

TEST_CASE("skips whitespace till EoF", "[lexer]"){
    torq::Lexer l("(           ");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::EOS );
}

TEST_CASE("skips comments to EoL", "[lexer]"){
    torq::Lexer l("(# this is a comment\n(");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::EOS );
}

TEST_CASE("skips comments to EoF", "[lexer]"){
    torq::Lexer l("(# this is a comment, no EoL");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::EOS );
}

TEST_CASE("skips multiline whitespace and comments combo", "[lexer]"){
    torq::Lexer l("(     # this is a comment\n    \t\n#another comment\n    )\n");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::ENDL );

    t = l.next();
    REQUIRE( t.type == torq::RPAREN );

    t = l.next();
    REQUIRE( t.type == torq::ENDL );

    t = l.next();
    REQUIRE( t.type == torq::EOS );
}

TEST_CASE("handles hex literals", "[lexer]"){
    torq::Lexer l("0xdeadbeef");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::INTEGER_LIT );
    REQUIRE( t.i_value == 0xdeadbeef );
}

TEST_CASE("handles hex literals with underscores", "[lexer]"){
    torq::Lexer l("0xdead_beef");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::INTEGER_LIT );
    REQUIRE( t.i_value == 0xdeadbeef );
}

TEST_CASE("handles hex literals with trailing token", "[lexer]"){
    torq::Lexer l("0xdeadbeef)");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::INTEGER_LIT );
    REQUIRE( t.i_value == 0xdeadbeef );

    t = l.next();
    REQUIRE( t.type == torq::RPAREN );
}

TEST_CASE("errors on bad hex literals", "[lexer]"){
    torq::Lexer l("0x)");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::ERROR );
}

TEST_CASE("handles binary literals", "[lexer]"){
    torq::Lexer l("0b0111");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::INTEGER_LIT );
    REQUIRE( t.i_value == 7 );
}

TEST_CASE("handles binary literals with underscores", "[lexer]"){
    torq::Lexer l("0b0000_0011");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::INTEGER_LIT );
    REQUIRE( t.i_value == 3 );
}

TEST_CASE("handles binary literals with trailing token", "[lexer]"){
    torq::Lexer l("0b0000_0111)");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::INTEGER_LIT );
    REQUIRE( t.i_value == 7 );

    t = l.next();
    REQUIRE( t.type == torq::RPAREN );
}

TEST_CASE("errors on bad binary literals", "[lexer]"){
    torq::Lexer l("0b 0b0123)");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::ERROR );

    t = l.next();
    REQUIRE( t.type == torq::INTEGER_LIT );
    REQUIRE( t.i_value == 1 );
}

TEST_CASE("decimal literals", "[lexer]"){
    torq::Lexer l("0123 343");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::INTEGER_LIT );
    REQUIRE( t.i_value == 123 );

    t = l.next();
    REQUIRE( t.type == torq::INTEGER_LIT );
    REQUIRE( t.i_value == 343 );
}

TEST_CASE("bad decimal literals", "[lexer]"){
    torq::Lexer l("0123x");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::INTEGER_LIT );
    REQUIRE( t.i_value == 123 );

    t = l.next();
    REQUIRE( t.type == torq::IDENTIFIER );
}

TEST_CASE("float literals", "[lexer]"){
    torq::Lexer l("0.55 3.14159 3e08 2.95E-09 4.5E+30");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::FLOAT_LIT );
    REQUIRE( t.f_value == 0.55 );

    t = l.next();
    REQUIRE( t.type == torq::FLOAT_LIT );
    REQUIRE( t.f_value == 3.14159 );

    t = l.next();
    REQUIRE( t.type == torq::FLOAT_LIT );
    REQUIRE( t.f_value == 3e08 );

    t = l.next();
    REQUIRE( t.type == torq::FLOAT_LIT );
    REQUIRE( t.f_value == 2.95E-09 );

    t = l.next();
    REQUIRE( t.type == torq::FLOAT_LIT );
    REQUIRE( t.f_value == 4.5e30 );
}

TEST_CASE("bad float literals", "[lexer]"){
    torq::Lexer l("3.14.159 45e 123e- 123ef");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::FLOAT_LIT );
    REQUIRE( t.f_value == 3.14 );

    t = l.next();
    REQUIRE( t.type == torq::DOT );

    t = l.next();

    t = l.next();
    REQUIRE( t.type == torq::ERROR );

    //123e-
    t = l.next();
    REQUIRE( t.type == torq::ERROR );

    //the '-' will be left
    t = l.next();
    REQUIRE( t.type == torq::MINUS );

    //123ef
    t = l.next();
    REQUIRE( t.type == torq::ERROR );
}

TEST_CASE("single line strings", "[lexer]"){
    torq::Lexer l(R"( "this is a test" "newline\n\\\ttest" )");

    torq::Token t = l.next();
    INFO(t.s_value);
    REQUIRE( t.type == torq::STRING_LIT );
    REQUIRE( t.s_value == "this is a test" );

    t = l.next();
    INFO(t.s_value);
    REQUIRE( t.type == torq::STRING_LIT );
    REQUIRE( t.s_value == "newline\n\\\ttest" );
}

TEST_CASE("multi-line string", "[lexer]"){
    torq::Lexer l(" \"\"\"this is a test\nwith multiple lines\nof text\"\"\" ");

    torq::Token t = l.next();
    INFO(t.s_value);
    REQUIRE( t.type == torq::STRING_LIT );
    REQUIRE( t.s_value == "this is a test\nwith multiple lines\nof text" );
}

TEST_CASE("bad single line strings", "[lexer]"){
    torq::Lexer l(" \"this is a test\n \"unterminated string at eof");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::ERROR );

    t = l.next();
    REQUIRE( t.type == torq::ERROR );
}

TEST_CASE("keywords", "[lexer]"){
    torq::Lexer l("if then else end while for do break return import true false int float string fn");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::IF );

    t = l.next();
    REQUIRE( t.type == torq::THEN );

    t = l.next();
    REQUIRE( t.type == torq::ELSE );

    t = l.next();
    REQUIRE( t.type == torq::END );

    t = l.next();
    REQUIRE( t.type == torq::WHILE );

    t = l.next();
    REQUIRE( t.type == torq::FOR );

    t = l.next();
    REQUIRE( t.type == torq::DO );

    t = l.next();
    REQUIRE( t.type == torq::BREAK );

    t = l.next();
    REQUIRE( t.type == torq::RETURN );

    t = l.next();
    REQUIRE( t.type == torq::IMPORT );

    t = l.next();
    REQUIRE( t.type == torq::TRUE_LIT );

    t = l.next();
    REQUIRE( t.type == torq::FALSE_LIT );

    t = l.next();
    REQUIRE( t.type == torq::INT_TYPE );

    t = l.next();
    REQUIRE( t.type == torq::FLOAT_TYPE );

    t = l.next();
    REQUIRE( t.type == torq::STRING_TYPE );

    t = l.next();
    REQUIRE( t.type == torq::FUNCTION );
}

TEST_CASE("identifiers", "[lexer]"){
    torq::Lexer l("fred _range TESTVAL");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::IDENTIFIER );
    REQUIRE( t.s_value == "fred");

    t = l.next();
    REQUIRE( t.type == torq::IDENTIFIER );
    REQUIRE( t.s_value == "_range");

    t = l.next();
    REQUIRE( t.type == torq::IDENTIFIER );
    REQUIRE( t.s_value == "TESTVAL");
}

TEST_CASE("lexer test file", "[lexer]"){

    std::string msg;

    std::ifstream test_file("../tests/data/data_002.tq");
    torq::Lexer l(test_file);

    while(true) {
        torq::Token t = l.next();

        if(t.type == torq::EOS)
            break;

        msg = std::format("error on line: {} at column: {}", t.line, t.column);

        INFO(msg);
        REQUIRE( t.type != torq::ERROR);
    }
}
