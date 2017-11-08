#include <iostream>
#include "src/block.hpp"
#include <string>
#include "src/tools.hpp"
#include <vector>

int main(int argc, const char * argv[]) {
    
    
    
    string filename("/Users/kristiankozak/Dropbox/Dokumenty_MUNI/BIT3/SecCode/proj/monero_test/extract_hash/valid_block.bin");
    
    filename = "/Users/kristiankozak/Dropbox/Dokumenty_MUNI/BIT3/SecCode/proj/PA193_test_parser_monero/external/blockExamples/higherBlock_0_1002920.bin";
    
    Block b = Block();
    b.init_from_file(filename);
    
    unsigned char hash[HASH_SIZE];
    b.get_block_hash(hash);
    
    for (int i = 0; i < HASH_SIZE; i++) {
        printf("%02x ", hash[i]);
    }
    std::cout << std::endl;
    
    // insert code here...
    std::cout << "Hello, PA193!\n";
    return 0;
}
