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

    Parser::Parser()
    {
        higher_block = new Block();
        lower_block = new Block();
    }

    Parser::~Parser()
    {
        delete higher_block;
        delete lower_block;
    }

int Parser::validate_block(string lower_filename, string higher_filename) {
    
    this->lower_block->init_from_file(lower_filename);
    this->higher_block->init_from_file(higher_filename);
    
    unsigned char lower_hash[HASH_SIZE];
    if(this->lower_block->get_block_hash(lower_hash))
    {
        std::cerr << "Parser: Failed to obtain hash result of lower block." << std::endl;
        return ERR_PARSER_FAILED_TO_VALIDATE;
    }

    unsigned char higher_hash[HASH_SIZE];
    if(this->higher_block->get_block_hash(higher_hash))
    {
        std::cerr << "Parser: Failed to obtain hash result of higher block." << std::endl;
        return ERR_PARSER_FAILED_TO_VALIDATE;
    }

    int result = memcmp(lower_hash, higher_hash, HASH_SIZE) ? 1 : 0;


    if(this->lower_block->clear_block())
    {
        std::cerr << "Parser: Failed to clear lower block." << std::endl;
        return ERR_PARSER_FAILED_TO_CLEAR;
    }

    if(this->higher_block->clear_block())
    {
        std::cerr << "Parser: Failed to clear higher block." << std::endl;
        return ERR_PARSER_FAILED_TO_CLEAR;
    }

    return result;
}
