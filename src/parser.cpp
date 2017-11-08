//
//  parser.cpp
//  parser
//
//  Created by Kristián Kozák on 07/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#include "parser.hpp"
#include "config.hpp"
#include "error.hpp"

#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>


int Parser::validate_block(string lower_filename, string higher_filename, bool print_hashes /* = false */) {

    if(lower_block.init_from_file(lower_filename) < 0 or
       higher_block.init_from_file(higher_filename) < 0) {
        std::cerr << "Parser: Failed to initialize block." << std::endl;
        return ERR_PARSER_FAILED_TO_INIT;
    }

    unsigned char lower_hash[HASH_SIZE];
    if(lower_block.get_block_hash(lower_hash)) {
        std::cerr << "Parser: Failed to obtain hash result of lower block." << std::endl;
        return ERR_PARSER_FAILED_TO_VALIDATE;
    }

    const unsigned char * higher_hash= higher_block.get_prev_id();

    if (print_hashes) {
        printf("Lower hash:  ");
        for(int i = 0; i < HASH_SIZE; i++) printf("%02x", lower_hash[i]);
        printf("\n");
        printf("Higher hash: ");
        for(int i = 0; i < HASH_SIZE; i++) printf("%02x", higher_hash[i]);
        printf("\n");
    }
    
    int result = (memcmp(lower_hash, higher_hash, HASH_SIZE)) ? 1 : 0;
    if (result == 0) {
        std::cout << "OK - The block is valid." << std::endl;
    } else {
        std::cout << "ERR - The block is not valid." << std::endl;
    }

    if(lower_block.clear_block() < 0 or
       higher_block.clear_block() < 0) {
        std::cerr << "Parser: Failed to clear a block." << std::endl;
        return ERR_PARSER_FAILED_TO_CLEAR;
    }

    return result;
}
