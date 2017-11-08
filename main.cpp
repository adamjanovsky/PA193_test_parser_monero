#include <iostream>
#include <string>
#include "src/parser.hpp"

int main(int argc, const char * argv[]) {
    
    if (argc != 3) {
        std::cerr << "Usage: parser <block_to_validate> <valid_block>" << std::endl;
        return 1;
    }
    
    // string f_higher = "/Users/kristiankozak/Dropbox/Dokumenty_MUNI/BIT3/SecCode/proj/PA193_test_parser_monero/external/blockExamples/higherBlock_0_1002920.bin";
    
    // string f_lower = "/Users/kristiankozak/Dropbox/Dokumenty_MUNI/BIT3/SecCode/proj/PA193_test_parser_monero/external/blockExamples/lowerBlock_0_1002919.bin";
    
    string f_lower(argv[1]);
    string f_higher(argv[2]);

    Parser p = Parser();
    
    p.validate_block(f_lower, f_higher, true);
    
    return 0;
}
