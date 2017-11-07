//
//  parser.cpp
//  parser
//
//  Created by Kristián Kozák on 07/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#include "parser.hpp"

int Parser::validate_block(string lower_filename, string higher_filename) {
    
    this->lower_block->init_from_file(lower_filename);
    this->higher_block->init_from_file(higher_filename);
    
    // validate

    this->lower_block->clear_block();
    this->higher_block->clear_block();

    return 0;
}
