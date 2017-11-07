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

#include "block.hpp"
#include "tools.hpp"
#include "error.hpp"
#include "config.hpp"

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
    
    // Open the in file
    std::ifstream in_file(filename);
    if (!in_file.is_open()) {
        std::cout << "Could not open file: " << filename << std::endl;
        return ERR_BL_INIT_FILE_ERROR;
    }
    this->filename = filename;

    // Try to parse header, miner_tx and tx_hashes
    
    if (load_header(in_file) < 0) {
        std::cerr << "Block header: Could not parse block header." << std::endl;
        return ERR_BL_INIT_HEADER;
    }
    
    if (load_miner_tx(in_file) < 0) {
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

int Block::load_miner_tx(ifstream & in)
{
    if(!in.good())
        {
            std::cerr << "Transaction parser: invalid stream." << std::endl;
            return ERR_TRANS_PARSER_ERROR;
        }

    int startPosition = in.tellg();
    unsigned char flag = 0;

    // Read the version flag
    in.read((char *) &flag, 1);
    if(flag != 0x01)
        {
            std::cerr << "Transaction parser: corrupted version flag." << std::endl;
            return ERR_TRANS_CORRUPTED_FORMAT;
        }

    if(!in.good())
        {
            std::cerr << "Transaction parser: invalid stream." << std::endl;
            return ERR_TRANS_PARSER_ERROR;
        }


    // skip unlock varint
    if(tools::skip_varint(in) < 0)
        {
            std::cerr << "Transaction parser: failed to skip varint." << std::endl;
            return ERR_TRANS_PARSER_ERROR;
        }


    // read number of inputs
    in.read((char *) &flag, 1);
    if(flag != 0x01)
        {
            std::cerr << "Transaction parser: corrupted num of input transactions flag." << std::endl;
            return ERR_TRANS_CORRUPTED_FORMAT;
        }

    if(!in.good())
        {
            std::cerr << "Transaction parser: invalid stream." << std::endl;
            return ERR_TRANS_PARSER_ERROR;
        }

    // read format of input trans
    in.read((char *) &flag, 1);
    if( flag != 0xff)
        {
            std::cerr << "Transaction parser: corrupted variant flag of input trans." << std::endl;
            return ERR_TRANS_CORRUPTED_FORMAT;
        }

    if(!in.good())
        {
            std::cerr << "Transaction parser: invalid stream." << std::endl;
            return ERR_TRANS_PARSER_ERROR;
        }

    // skip input trans
    if(tools::skip_varint(in) < 0)
        {
            std::cerr << "Transaction parser: failed to skip varint." << std::endl;
            return ERR_TRANS_PARSER_ERROR;
        }

    if(!in.good())
        {
            std::cerr << "Transaction parser: invalid stream." << std::endl;
            return ERR_TRANS_PARSER_ERROR;
        }

    // read number of output trans
    in.read((char *) &flag, 1);
    int numOfTxOut = (int) flag;

    // read all output trans
    for(int i = 0; i < numOfTxOut; i++)
        {
            if(!in.good())
                {
                    std::cerr << "Transaction parser: invalid stream." << std::endl;
                    return ERR_TRANS_PARSER_ERROR;
                }

            // amount
            if(tools::skip_varint(in) < 0)
                {
                    std::cerr << "Transaction parser: failed to skip varint." << std::endl;
                    return ERR_TRANS_PARSER_ERROR;
                }

            in.read((char *) &flag, 1);

            // flag specifying pub key format
            if(flag != 0x02)
                {
                    std::cerr << "Transaction parser: corrupted tx_out flag." << std::endl;
                    return ERR_TRANS_CORRUPTED_FORMAT;
                }

            if(!in.good())
                {
                    std::cerr << "Transaction parser: invalid stream." << std::endl;
                    return ERR_TRANS_PARSER_ERROR;
                }

            // hash of pub key
            if(tools::skip_bytes(in, HASH_SIZE) != HASH_SIZE)
                {
                    std::cerr << "Transaction parser: failed to skip tx_out hash." << std::endl;
                    return ERR_TRANS_PARSER_ERROR;
                }
        }
    if(!in.good())
        {
            std::cerr << "Transaction parser: invalid stream." << std::endl;
            return ERR_TRANS_PARSER_ERROR;
        }


    // flag, number of extras
    in.read((char *) &flag, 1);
    int numOfExtras = (int) flag;

    if(!in.good())
        {
            std::cerr << "Transaction parser: invalid stream." << std::endl;
            return ERR_TRANS_PARSER_ERROR;
        }

    if(tools::skip_bytes(in, numOfExtras) != numOfExtras)
    {
        std::cerr << "Transaction parser: Failed to skip extras." << std::endl;
        return ERR_TRANS_PARSER_ERROR;
    }

    int endPosition = in.tellg();
    in.seekg(startPosition);

    this->miner_tx = new(std::nothrow) unsigned char[(endPosition - startPosition) * sizeof(unsigned char)];
    if(!this->miner_tx)
    {
        std::cerr << "Miner tx: Out of memory." << std::endl;
        return ERR_TRANS_OUT_OF_MEM;
    }

    in.read((char *) this->miner_tx, endPosition - startPosition);
    miner_tx_length = endPosition - startPosition;
    
    return OK;
}

int Block::load_tx_hashes(ifstream & in)
{
    if(!in.good())
    {
        std::cerr << "Tx hashes: invalid stream." << std::endl;
        return ERR_TXH_PARSER_ERROR;
    }

    unsigned char flag = 0;
    in.read((char *) &flag, 1);
    this->tx_hashes_count = flag;

    this->tx_hashes = new(std::nothrow) unsigned char[(HASH_SIZE * this->tx_hashes_count) * sizeof(unsigned char)];
    if(!this->tx_hashes)
    {
        std::cerr << "Tx hashes: Out of memory." << std::endl;
        return ERR_TRANS_OUT_OF_MEM;
    }

    for(int i = 0; i < this->tx_hashes_count; i++)
    {
        if(!in.good())
        {
            std::cerr << "Tx hashes: invalid stream." << std::endl;
            return ERR_TXH_PARSER_ERROR;
        }
        in.read((char *) (this->tx_hashes + HASH_SIZE * i), HASH_SIZE);
    }

    
    // Try to read one more byte and test EOF
    char c;
    in.read(&c, 1);
    if(!in.eof()) // some bytes remain, weird?
    {
        std::cerr << "Tx hashes: Blocked parsed but some bytes are remaining." << std::endl;
        return ERR_TXH_NO_EOF;
    }

    return OK;
}

int Block::get_block_hash(unsigned char * hash) {
    unsigned char miner_tx_hash[HASH_SIZE];
    tools::hash(miner_tx, miner_tx_length, miner_tx_hash);
    
    std::vector<unsigned char> all_hashes_vect;
    
    // push back the miner_tx hash
    all_hashes_vect.insert(all_hashes_vect.end(), &miner_tx_hash[0], &miner_tx_hash[HASH_SIZE]);
    // push back the other hashes
    all_hashes_vect.insert(all_hashes_vect.end(), &tx_hashes[0], &tx_hashes[tx_hashes_count * HASH_SIZE]);

    // get the tree root hash
    unsigned char tree_root_hash[HASH_SIZE];
    tools::tree_hash(all_hashes_vect.data(), all_hashes_vect.size(), tree_root_hash);

    // blob that will be hashed to get the block hash
    std::vector<unsigned char>hashing_blob;
    // push back the block header
    hashing_blob.insert(hashing_blob.end(), &block_header[0], &block_header[block_header_length]);
    // push back the tree root hash
    hashing_blob.insert(hashing_blob.end(), &tree_root_hash[0], &tree_root_hash[HASH_SIZE]);
    
    // count all transactions including the miner tx
    unsigned long all_tx_count = 1 + tx_hashes_count;
    // append it serialized as varint to the blob
    tools::write_varint(hashing_blob, all_tx_count);
    
    unsigned char vect_size = static_cast<unsigned char>(hashing_blob.size());
    hashing_blob.insert(hashing_blob.begin(), vect_size);
    
    tools::hash(hashing_blob.data(), hashing_blob.size(), hash);
    
    // tmp: print blob
    printf("{ ");
    for (auto item : hashing_blob) {
        printf("%02x ", item);
    }
    printf(" } \n");
    
    return OK;
}


int Block::clear_block() {
    initialized = false;
    filename = "";
    
    if (block_header and block_header != nullptr) delete [] block_header;
    if (miner_tx and miner_tx != nullptr) delete [] miner_tx;
    if (tx_hashes and tx_hashes != nullptr) delete [] tx_hashes;
    
    memset(prev_id, 0, HASH_SIZE);

    block_header_length = 0;
    tx_hashes_count = 0;
    
    return OK;
}
