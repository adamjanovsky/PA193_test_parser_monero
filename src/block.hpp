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
    // true if the block has been successfully initialized
    bool initialized;
    
    // Filename to read the block from
    string filename;
    
    // Hash of the previous block in blockchain
    unsigned char prev_id[HASH_SIZE];
    
    // Header of this block
    unsigned char * block_header;
    // Length of the header
    size_t block_header_length;
    
    // Extracted miner transaction data
    unsigned char * miner_tx;
    // Size of miner transaction
    size_t miner_tx_length;
    
    // Transaction hashes
    unsigned char * tx_hashes;
    // Count of transaction hashes
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
    
    /**
     * @brief load_tx_hashes loads array of hashes into variable and stores how many hashes are present as well
     * @param in the input stream with correct offset
     * @return 0 if ok, neg if not
     */
    int load_tx_hashes(ifstream & in);
    
public:
    
    Block() :
    initialized(false),
    filename(""),
    block_header(nullptr),
    block_header_length(0),
    miner_tx(nullptr),
    miner_tx_length(0),
    tx_hashes(nullptr),
    tx_hashes_count(0) {};
    
    /**
     Init and parse the block from the specified file.

     @param filename filename of the file to read the block from
     @return 0 if OK, negative otherwise
     */
    int init_from_file(string filename);
    
    /**
     Return hash of the previous block (previous block ID) extracted from header of this block.

     @return previous block id (hash)
     */
    const unsigned char * get_prev_id() {
        return prev_id;
    }
    
    /**
     Calculate and return hash of this block

     @param hash where to store the hash
     @return 0 if ok, negative otherwise
     */
    int get_block_hash(unsigned char * hash);
    
    /**
     Clear and dealloc block data.

     @return OK
     */
    int clear_block();
};


#endif /* block_hpp */
