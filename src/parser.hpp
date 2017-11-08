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
    // Higher block, this one is assumed valid
    Block higher_block;
    
    // Lower block, this one will be validated
    Block lower_block;
    
public:
    
    /**
     Takes filenames of two blocks, one to validate and one assumed valid. Validates the block to validate and returns the result.

     @param lower_filename where the block to validate is saved
     @param higher_filename where the block that is assumed valid is saved
     @param print_hashes set to true if you want to print out the extracted/calculated hash values
     @return 0 if OK, 1 if block is not valid, negative if one of the blocks could not be parsed
     */
    int validate_block(string lower_filename, string higher_filename, bool print_hashes = false);
};

#endif /* parser_hpp */
