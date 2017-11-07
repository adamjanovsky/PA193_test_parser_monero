//
//  block.cpp
//  parser
//
//  Created by Kristián Kozák on 07/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#include <fstream>
#include <iostream>

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

    if(!in.eof()) // some bytes remain, weird?
    {
        std::cerr << "Tx hashes: Blocked parsed but some bytes are remaining." << std::endl;
    }


    return OK;
}
