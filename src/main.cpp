#include <fstream>
#include <iostream>

#include "clipp.h"


int main(int argc, char* argv[]) {
    bool disasm = false;
    std::string infile = "";

    auto cli = (
        clipp::option("-d", "--disasm").set(disasm).doc("disassemble code"),
        clipp::value("input file", infile)
    );

    if(!clipp::parse(argc, argv, cli)) {
        std::cout << clipp::make_man_page(cli, argv[0]);
        return 0;
    }

    std::ifstream source(infile);
    if(!source.good()){
        std::cout << "Source file '" << infile << "' not found.\nExiting...\n\n";
        return -1;
    }
}
