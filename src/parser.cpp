//
//  parser.cpp
//  parser
//
//  Created by Kristián Kozák on 07/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#include "parser.hpp"
#include "hash.hpp"
#include "error.hpp"

#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>

int Parser::validate_block(string lower_filename, string higher_filename, bool print_hashes /* = false */) {
    // Initialize the blocks from given filenames
    if(lower_block.init_from_file(lower_filename) < 0 or
       higher_block.init_from_file(higher_filename) < 0) {
        std::cerr << "Parser: Failed to initialize block." << std::endl;
        return ERR_PARSER_FAILED_TO_INIT;
    }

    // Get the block hash and block id from the next block
    hash::hash_t lower_hash;
    hash::hash_t higher_prev_id;
    if(lower_block.get_block_hash(lower_hash) or higher_block.get_prev_id(higher_prev_id)) {
        std::cerr << "Parser: Failed to obtain hash result of lower block." << std::endl;
        return ERR_PARSER_FAILED_TO_VALIDATE;
    }

    // Print the hashes if desired
    if (print_hashes) {
        printf("Lower hash:  ");
        for(int i = 0; i < lower_hash.size(); i++) printf("%02x", lower_hash[i]);
        printf("\n");
        printf("Higher hash: ");
        for(int i = 0; i < higher_prev_id.size(); i++) printf("%02x", higher_prev_id[i]);
        printf("\n");
    }
    
    // Compare the hash values to validate the block
    if (memcmp(lower_hash.data(), higher_prev_id.data(), lower_hash.size()) == 0) {
        std::cout << "OK - The block is valid." << std::endl;
        return OK;
    } else {
        std::cout << "ERR - The block is not valid." << std::endl;
        return ERR_PARSER_BLOCK_INVALID;
    }
}
