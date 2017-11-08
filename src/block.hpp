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
#include "hash.hpp"
#include <fstream>
#include <vector>
#include <array>

using std::string;
using std::ifstream;

class Block {
    // true if the block has been successfully initialized
    bool initialized;
    
    // Filename to read the block from
    string filename;
    
    // Hash of the previous block in blockchain
    std::array<unsigned char, HASH_SIZE> prev_id;
    
    // Header of this block
    std::vector<unsigned char> block_header;
    
    // Extracted miner transaction data
    std::vector<unsigned char> miner_tx_data;
    // version of miner_tx
    unsigned int miner_tx_version;
    
    // Transaction hashes
    std::vector<unsigned char> tx_hashes;
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

    /**
     * @brief get_miner_tx_hash computes the hash of the transaction based on its version and copies it into unsigned array hash
     * @param miner_tx_hash - the unsigned array to store the result into
     * @return 0 if ok, neg if not
     */
    int get_miner_tx_hash(std::array<unsigned char, HASH_SIZE> & miner_tx_hash);
    
public:
    
    Block() :
    initialized(false),
    filename(""),
    miner_tx_version(0),
    tx_hashes_count(0) {};
    
    /**
     Init and parse the block from the specified file.

     @param filename filename of the file to read the block from
     @return 0 if OK, negative otherwise
     */
    int init_from_file(string filename);
    
    /**
     Return hash of the previous block extracted from the block header.

     @param prev_id_copy copy of hash of the previous block
     @return 0 if ok, negative if error (block unitialized)
     */
    int get_prev_id(std::array<unsigned char, HASH_SIZE> & prev_id_copy);
    
    /**
     Calculate and return hash of this block

     @param block_hash where to store the hash
     @return 0 if ok, negative otherwise
     */
    int get_block_hash(std::array<unsigned char, HASH_SIZE> & block_hash);
};


#endif /* block_hpp */
