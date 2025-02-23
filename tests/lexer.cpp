#define CONFIG_CATCH_MAIN

#include <catch2/catch_test_macros.hpp>

#include <fstream>

#include "../src/parser/lexer.hpp"

TEST_CASE("Lexer extracts single characters", "[lexer]") {

    torq::Lexer l("()=.==\n");
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
    REQUIRE( t.type == torq::ENDL );
}

TEST_CASE("Lexer return EOF token", "[lexer]") {
    torq::Lexer l("(");

    torq::Token t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::EOS );
}

TEST_CASE("Lexer handlers unwanted characters", "[lexer]") {
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
