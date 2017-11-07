//
//  parser.hpp
//  parser
//
//  Created by Kristián Kozák on 07/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#ifndef parser_hpp
#define parser_hpp

#include <stdio.h>
#include "block.hpp"
#include <string>

using std::string;

class Parser {
    Block * higher_block;
    Block * lower_block;
    
public:
    Parser() {
        // alloc blocks
    }
    
    ~Parser() {
        // free blocks
    }
    
    // proste inicializuj a validuj dva blocky
    int validate_block(string lower_filename, string higher_filename);
};

#endif /* parser_hpp */
