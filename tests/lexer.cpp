#define CONFIG_CATCH_MAIN

#include <catch2/catch_test_macros.hpp>

#include <fstream>

#include "../src/parser/lexer.hpp"

TEST_CASE("Lexer extracts single characters", "[lexer]") {

    torq::Lexer l("(");
    torq::Token t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    torq::Lexer l2(")");
    t = l2.next();
    REQUIRE( t.type == torq::RPAREN );

    torq::Lexer l3("\n");
    t = l3.next();
    REQUIRE( t.type == torq::NL );
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
    REQUIRE( t.type == torq::NL );

    t = l.next();
    REQUIRE( t.type == torq::RPAREN );

    t = l.next();
    REQUIRE( t.type == torq::LPAREN );

    t = l.next();
    REQUIRE( t.type == torq::NL );

    t = l.next();
    REQUIRE( t.type == torq::EOS );
}
