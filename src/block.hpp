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
#include <fstream>

using std::string;
using std::ifstream;

class Block {
    bool initialized;
    
    // Filename to read the block from
    string filename;
    
    // Hash of the previous block in blockchain
    unsigned char prev_id[HASH_SIZE];
    
    // Hash of this block
    unsigned char block_hash[HASH_SIZE];
    
    // Header of this block
    unsigned char * block_header;
    
    // Length of the header
    size_t block_header_length;
    
    
    unsigned char * miner_tx;
    unsigned char * tx_hashes;
    unsigned long tx_hashes_count;
    

    /**
     Examines the header of the block, loads it into the block_header, extracts hash to prev_id and save the header length to block_header_length.

     @param in ifstream where the block is saved (function will jump to the beginning)
     @return 0 if ok, neg if not
     */
    int load_header(ifstream & in);
        
    /**
     * @brief load_miner_tx examines the miner transaction, loads it into miner_tx field.
     * @param in ifstream where the transaction is saved at correct offset
     * @return 0 if ok, neg if not
     */
    int load_miner_tx(ifstream & in);
    
    // load array of tx hashes
    int load_tx_hashes(ifstream & in);
    
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
