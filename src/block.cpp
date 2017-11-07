//
//  block.cpp
//  parser
//
//  Created by Kristián Kozák on 07/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#include "block.hpp"
#include <fstream>
#include <iostream>
#include "error.hpp"
#include "tools.hpp"

int Block::load_header(ifstream & in) {
    // To be sure, move to the beginning of the stream
    in.seekg(in.beg);
    
    // Skip the three varints at the beginning
    // (major version, minor version, timestamp)
    for (int i = 0; i < 3; i++) {
        if (tools::skip_varint(in) < 0) {
            std::cerr << "Block header: Invalid format." << std::endl;
            return ERR_BL_LOADHDR_INVALID_FORMAT;
        }
    }
    
    // Alloc and load the hash
    in.read((char *)prev_id, HASH_SIZE);
    if (!in.good()) {
        std::cerr << "Block header: Unexpected end of file." << std::endl;
        return ERR_BL_LOADHDR_EOF;
    }
    
    // Skip the nonce at the end of the header
    if (tools::skip_bytes(in, 4) < 0) {
        std::cerr << "Block header: Invalid format." << std::endl;
        return ERR_BL_LOADHDR_INVALID_FORMAT;
    }
    
    // Get the header length
    block_header_length = in.tellg();
    
    // Alloc space for the header
    this->block_header = new(std::nothrow) unsigned char[block_header_length];
    if (!this->block_header) {
        std::cerr << "Block header: Out of memory." << std::endl;
        return ERR_BL_LOADHDR_OUT_OF_MEM;
    }
    
    // Read the whole header
    in.seekg(in.beg);
    in.read((char *)this->block_header, block_header_length);
    
    // To be sure, check once more if we have loaded the header sucessfully
    if (!in.good()) {
        std::cerr << "Block header: Unexpected end of file." << std::endl;
        return ERR_BL_LOADHDR_EOF;
    }
    
    return OK;
}

int Block::init_from_file(string filename) {
    // alloc veci dle jejich delky v blocku
    // rozparsovat
    
    // predavat si istream, bacha, at je na konci fce pointer dycky na konci vyparsovane casti
    
    std::ifstream in_file(filename);
    if (!in_file.is_open()) {
        std::cout << "wtf" << std::endl;
    }
    
    load_header(in_file);
    
    return 0;
}

int Block::clear_block() {
    return 0;
}
