//
//  block.cpp
//  parser
//
//  Created by Kristián Kozák on 07/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>

#include "block.hpp"
#include "tools.hpp"
#include "error.hpp"
#include "hash.hpp"

int Block::get_prev_id(hash::hash_t & prev_id_copy) {
    if (this->initialized) {
        std::copy(prev_id.begin(), prev_id.end(), prev_id_copy.begin());
        return OK;
    } else {
        return ERR_BL_NOT_INITIALIZED;
    }
}

int Block::init_from_file(string filename) {
    // Set init to false to be sure the block stays unitialized if we encounter an error in the middle of init
    initialized = false;
    
    // Open the in file
    std::ifstream in_file(filename);
    if (!in_file.is_open()) {
        std::cout << "Could not open file: " << filename << std::endl;
        return ERR_BL_INIT_FILE_ERROR;
    }
    
    // Save the filename
    this->filename = filename;
    
    
    // Try to parse header, miner_tx and tx_hashes
    
    if (load_header(in_file) < 0 || !in_file.good()) {
        std::cerr << "Block header: Could not parse block header." << std::endl;
        return ERR_BL_INIT_HEADER;
    }
    
    if (load_miner_tx(in_file) < 0 || !in_file.good()) {
        std::cerr << "Block header: Could not parse miner transaction." << std::endl;
        return ERR_BL_INIT_MINER_TX;
    }
    
    if (load_tx_hashes(in_file) < 0) {
        std::cerr << "Block header: Could not parse transaction hashes." << std::endl;
        return ERR_BL_INIT_TX_HASHES;
    }
    
    // Everything went fine...
    initialized = true;
    return OK;
}


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
    in.read(reinterpret_cast<char *>(&prev_id[0]), prev_id.size());
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
    long long block_header_length = in.tellg();

    // Alloc space for the header
    block_header.resize(block_header_length, 0x00);

    // Read the whole header
    in.seekg(in.beg);
    in.read(reinterpret_cast<char *>(&block_header[0]), block_header_length);

    // To be sure, check once more if we have loaded the header sucessfully
    if (!in.good()) {
        std::cerr << "Block header: Unexpected end of file." << std::endl;
        return ERR_BL_LOADHDR_EOF;
    }

    return OK;
}

int Block::load_miner_tx(ifstream & in) {
    long long start_pos = in.tellg();

    in.read((char *) & this->miner_tx_version, 1);
    if(this->miner_tx_version != 0x01 and this->miner_tx_version != 0x02) {
        std::cerr << "Transaction parser: Wrong version of transaction" << std::endl;
        return ERR_TRANS_CORRUPTED_FORMAT;
    }

    // read and store miner_tx version

    // Check initial stamps and the input transaction:
    // (1) Skip unlock varint
    // (2) Check that there is only one input transaction
    // (3) Check format of input transaction (0xff = tf_gen)
    // (4) Skip input transaction (one varint indicating height of the block)
    if(tools::skip_varint(in) < 0           or
       tools::expect_byte(in, 0x01) < 0     or
       tools::expect_byte(in, 0xff) < 0     or
       tools::skip_varint(in) < 0) {
        std::cerr << "Transaction parser: Invalid header format." << std::endl;
        return ERR_TRANS_PARSER_ERROR;
    }

    // Read number of output trans
    unsigned long tx_out_count;
    if (tools::read_varint(in, tx_out_count) < 0) {
        std::cerr << "Transaction parser: Could not read count of output transactions." << std::endl;
        return ERR_TRANS_PARSER_ERROR;
    }

    // Read all output trans
    for(int i = 0; i < tx_out_count; i++)
    {
        // Skip the output transaction:
        // (1) Amount varint
        // (2) Check the key tag (0x02)
        // (3) Skip the key
        if(tools::skip_varint(in) < 0           or
           tools::expect_byte(in, 0x02) < 0     or
           tools::skip_bytes(in, hash::HASH_SIZE) != hash::HASH_SIZE) {
            std::cerr << "Transaction parser: Invalid header format." << std::endl;
            return ERR_TRANS_PARSER_ERROR;
        }
    }

    // Get number of extras
    unsigned long int extras_count;
    if(tools::read_varint(in, extras_count) < 0)
    {
        std::cerr << "Transaction parser: failed to read number of extras." << std::endl;
        return ERR_TRANS_PARSER_ERROR;
    }

    // Skip the extras
    if(tools::skip_bytes(in, static_cast<unsigned int>(extras_count)) != static_cast<int>(extras_count))
    {
        std::cerr << "Transaction parser: Failed to skip extras." << std::endl;
        return ERR_TRANS_PARSER_ERROR;
    }

    // Get the position and calculate the complete length
    long long end_pos = in.tellg();
    in.seekg(start_pos);
    long long miner_tx_length = end_pos - start_pos;

    // Read the entire miner_tx to memory
    miner_tx_data.resize(miner_tx_length, 0x00);
    in.read(reinterpret_cast<char *>(&miner_tx_data[0]), miner_tx_length);

    // v2: skip one zero byte
    if(this->miner_tx_version == 2) {
        if(tools::expect_byte(in, 0x00)) {
            std::cerr << "Transaction parser: Invalid header format." << std::endl;
            return ERR_TRANS_PARSER_ERROR;
        }
    }

    return OK;
}

int Block::load_tx_hashes(ifstream & in) {
    // Read count of tx hashes
    if (tools::read_varint(in, tx_hashes_count) < 0) {
        std::cerr << "Tx hashes: could not read count of hashes." << std::endl;
        return ERR_TXH_PARSER_ERROR;
    }

    // Alloc memory
    tx_hashes.resize(tx_hashes_count * hash::HASH_SIZE, 0x00);

    // Read the hashes
    in.read(reinterpret_cast<char *>(&tx_hashes[0]), tx_hashes.size());
    if(!in.good()) {
        std::cerr << "Tx hashes: invalid stream." << std::endl;
        return ERR_TXH_PARSER_ERROR;
    }

    // Try to read one more byte and test EOF
    char c;
    in.read(&c, 1);
    if(!in.eof()) {
        // still some bytes to read, weird
        std::cerr << "Tx hashes: Blocked parsed but some bytes are remaining." << std::endl;
        return ERR_TXH_NO_EOF;
    }

    return OK;
}

int Block::get_miner_tx_hash(hash::hash_t & miner_tx_hash) {
    switch (this->miner_tx_version) {
        case 1:
            tools::hash(miner_tx_data.data(), miner_tx_data.size(), &miner_tx_hash[0]);
            return OK;
        case 2: {
            unsigned char to_hash[3 * hash::HASH_SIZE];
            
            // to_hash[0]
            tools::hash(miner_tx_data.data(), miner_tx_data.size(), to_hash);
            
            // to_hash[1]
            unsigned char zero_byte[1] = {0x00};
            tools::hash(zero_byte, 1, to_hash + hash::HASH_SIZE);
            
            // to_hash[2]
            memset(to_hash + 2 * hash::HASH_SIZE, 0, hash::HASH_SIZE);
            
            tools::hash(to_hash, 3 * hash::HASH_SIZE, &miner_tx_hash[0]);
            return OK;
        }
        default:
            return ERR_BL_HASH_TRANS_WRONG_VERSION;
    }
}

int Block::get_block_hash(hash::hash_t & block_hash) {
    // Exit if the block is not initialized
    if (this->initialized == false) {
        return ERR_BL_NOT_INITIALIZED;
    }
    
    hash::hash_t miner_tx_hash;

    if(this->get_miner_tx_hash(miner_tx_hash))
    {
        std::cerr << "Block hash: Failed to obtain miner_tx hash." << std::endl;
        return ERR_BL_HASH_MINER_TX_FAIL;
    }

    std::vector<unsigned char> all_hashes_vect;

    // push back the miner_tx hash
    all_hashes_vect.insert(all_hashes_vect.end(), miner_tx_hash.begin(), miner_tx_hash.end());
    // push back the other hashes
    all_hashes_vect.insert(all_hashes_vect.end(), tx_hashes.begin(), tx_hashes.end());

    // get the tree root hash
    hash::hash_t tree_root_hash;
    tools::tree_hash(all_hashes_vect, tree_root_hash);

    // blob that will be hashed to get the block hash
    std::vector<unsigned char>hashing_blob;
    // push back the block header
    hashing_blob.insert(hashing_blob.end(), block_header.begin(), block_header.end());
    // push back the tree root hash
    hashing_blob.insert(hashing_blob.end(), tree_root_hash.begin(), tree_root_hash.end());

    // count all transactions including the miner tx
    unsigned long all_tx_count = 1 + tx_hashes_count;
    // append it serialized as varint to the blob
    tools::write_varint(hashing_blob, all_tx_count);

    unsigned char vect_size = static_cast<unsigned char>(hashing_blob.size());
    hashing_blob.insert(hashing_blob.begin(), vect_size);

    tools::hash(hashing_blob.data(), hashing_blob.size(), &block_hash[0]);

    return OK;
}
