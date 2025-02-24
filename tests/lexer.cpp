#define CONFIG_CATCH_MAIN

#include <catch2/catch_test_macros.hpp>

#include <fstream>

#include "../src/parser/lexer.hpp"

TEST_CASE("Lexer extracts basic characters", "[lexer]") {

    torq::Lexer l("( ) = . == > >= < <= ; + - * / % \n");
    torq::Token t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::RPAREN );

    t = l.next();
    REQUIRE( t.type == torq::ASSIGN );

    t = l.next();
    REQUIRE( t.type == torq::DOT );

    t = l.next();
    REQUIRE( t.type == torq::EQUALS );

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

TEST_CASE("Lexer return EOF token", "[lexer]") {
    torq::Lexer l("(");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::EOS );
}

TEST_CASE("Lexer handles unwanted characters", "[lexer]") {
    char ch = 0xAE; //® - registered sign
    std::string source = std::string(1, ch);
    torq::Lexer l(source);

    torq::Token t = l.next();
    REQUIRE( t.type == torq::ERROR );
}

TEST_CASE("Lexer peeks correctly", "[lexer]") {
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

TEST_CASE("Lexer skips whitespace", "[lexer]"){
    torq::Lexer l("( )  \t\t   \n");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::RPAREN );

    t = l.next();
    REQUIRE( t.type == torq::ENDL );
}

TEST_CASE("Lexer skips whitespace till EoF", "[lexer]"){
    torq::Lexer l("(           ");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::EOS );
}

TEST_CASE("Lexer skips comments to EoL", "[lexer]"){
    torq::Lexer l("(# this is a comment\n");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::EOS );
}

TEST_CASE("Lexer skips comments to EoF", "[lexer]"){
    torq::Lexer l("(# this is a comment, no EoL");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::EOS );
}

TEST_CASE("Lexer skips multiline whitespace and comments combo", "[lexer]"){
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

TEST_CASE("Lexer handles hex literals", "[lexer]"){
    torq::Lexer l("0xdeadbeef");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::INTEGER );
    REQUIRE( t.i_value == 0xdeadbeef );
}

TEST_CASE("Lexer handles hex literals with underscores", "[lexer]"){
    torq::Lexer l("0xdead_beef");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::INTEGER );
    REQUIRE( t.i_value == 0xdeadbeef );
}

TEST_CASE("Lexer handles hex literals with trailing token", "[lexer]"){
    torq::Lexer l("0xdeadbeef)");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::INTEGER );
    REQUIRE( t.i_value == 0xdeadbeef );

    t = l.next();
    REQUIRE( t.type == torq::RPAREN );
}

TEST_CASE("Lexer errors on bad hex literals", "[lexer]"){
    torq::Lexer l("0x)");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::ERROR );
}

TEST_CASE("Lexer handles binary literals", "[lexer]"){
    torq::Lexer l("0b0111");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::INTEGER );
    REQUIRE( t.i_value == 7 );
}

TEST_CASE("Lexer handles binary literals with underscores", "[lexer]"){
    torq::Lexer l("0b0000_0011");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::INTEGER );
    REQUIRE( t.i_value == 3 );
}

TEST_CASE("Lexer handles binary literals with trailing token", "[lexer]"){
    torq::Lexer l("0b0000_0111)");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::INTEGER );
    REQUIRE( t.i_value == 7 );

    t = l.next();
    REQUIRE( t.type == torq::RPAREN );
}

TEST_CASE("Lexer errors on bad binary literals", "[lexer]"){
    torq::Lexer l("0b)");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::ERROR );
}
