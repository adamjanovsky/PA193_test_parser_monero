//
//  block.hpp
//  parser
//
//  Created by Kristián Kozák on 07/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#ifndef block_hpp
#define block_hpp

#include <stdio.h>
#include <string>
#include "config.hpp"
#include <istream>

using std::string;
using std::istream;

class Block {
    bool initialized;
    string filename;
    unsigned char prev_id[HASH_SIZE];
    unsigned char block_hash[HASH_SIZE];
    unsigned char * block_header;
    size_t block_header_length;
    unsigned char * miner_tx;
    unsigned char * tx_hashes;
    unsigned long tx_hashes_count;
    
    // Tady budou pomocny fce na parse blocku:
    // ----
    
    // load header lenght and the header itself
    // + load hash also!
    int load_header(istream & in);
    
    // load blob of miner tx
    int load_miner_tx(istream & in);
    
    // load array of tx hashes
    int load_tx_hashes(istream & in);
    
public:
    // bacha na inicializace pointeru
    Block() : initialized(0) {};
    
    // init from file, rozmrdat se
    int init_from_file(string filename);
    // hash previous blocku ulozenej v headeru
    const unsigned char * extract_prev_id() {
        return prev_id;
    }
    
    // zahashuj se a vrat hash
    const unsigned char * get_block_hash() {
        return block_hash;
    }
    
    // deallo shit
    int clear_block();
};


#endif /* block_hpp */
