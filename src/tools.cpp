//
//  hash.cpp
//  tree_hash_clear
//
//  Created by Kristián Kozák on 06/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#include <iostream>
#include <cstring>
#include <vector>
#include "keccak/Keccak-readable-and-compact.h"
#include "tools.hpp"
#include "hash.hpp"
#include "error.hpp"


int tools::expect_byte(std::istream & in, unsigned char expected_byte) {
    unsigned char c;
    in.read(reinterpret_cast<char *>(&c), 1);
    
    // Check for EOF
    if (!in.good()) {
        std::cerr << "Expect byte: Unexpected end of stream." << std::endl;
        return ERR_EXPBT_UNEXPECTED_EOF;
    }
    
    // Check for EOF
    if (c != expected_byte) {
        std::cerr << "Expect byte: Input " << std::hex << c;
        std::cerr << std::dec << " does not match expected input ";
        std::cerr << std::hex << expected_byte << std::dec << "." << std::endl;
        return ERR_EXPBT_DOES_NOT_MATCH;
    }
    
    return OK;
}

int tools::write_varint(std::vector<unsigned char> & out_vect, unsigned long val) {
    
    unsigned char c;
    
    while (val >= 0x80) {
        // get lowest 7 bits
        c = val & 0x7f;
        // save to the vector with the first bit set to 1
        out_vect.push_back(c | 0x80);
        // shift the value
        val >>= 7;
    }
    
    // write the last byte (with the first bit set to 0)
    c = val & 0x7f;
    out_vect.push_back(c);
    
    return OK;
}

int tools::read_varint(std::istream & in, unsigned long & val) {
    
    // We will load the varint into a vector and then deserialize it
    std::vector<unsigned char> ser_vect;
    unsigned char c;
    
    // 10 bytes is maximum for the varint to fit in 64 bit int
    // and the last serialized val must be 0x01 so that the int does not overflow
    // because each byte contains 7 bits and 9 * 7 + 1 = 63 + 1 = 64
    for (int i = 0; i < 10; i++) {
        in.read((char *)&c, 1);
        
        // Check for overflow
        if (i == 9 and c != 0x01) {
            std::cerr << "Read varint: Varint overflow." << std::endl;
            return ERR_READV_OVERFLOW;
        }
        
        // Check for EOF
        if (!in.good()) {
            std::cerr << "Read varint: Unexpected end of stream." << std::endl;
            return ERR_READV_UNEXPECTED_EOF;
        }
        
        // Strip the first bit and save it into the vector
        ser_vect.push_back(c & 0x7f);
        
        if ((c & 0x80) == 0x00) {
            // we have read the last byte
            break;
        }

    }
    
    val = 0;
    int last_index = static_cast<int>(ser_vect.size()) - 1;
    
    for (int i = last_index; i >= 0; i--) {
        val <<= 7;
        val |= ser_vect.at(i);
    }
    
    return OK;
}

int tools::skip_varint(std::istream & in) {
    unsigned char c;
    in.read((char *)&c, 1);
    int bytes_skipped = 1;

    // read from the stream until there was a byte that started with a zero bit
    while (c & 0x80 && bytes_skipped <= 10) {
        if (in.eof()) {
            std::cerr << "Skip Varint: Unexpected end of stream." << std::endl;
            return ERR_SKIPV_UNEXPECTED_EOF;
        }
        in.read((char *)&c, 1);
        bytes_skipped++;
    }
    
    if (bytes_skipped > 10) {
        std::cerr << "Skip Varint: String too long." << std::endl;
        return ERR_SKIPV_VARINT_TOO_LONG;
    }
    
    return bytes_skipped;
}

int tools::skip_bytes(std::istream & in, unsigned int length) {
    if (length == 0) {
        std::cerr << "Skip bytes: Invalid length." << std::endl;
        return ERR_SKIPB_INVALID_LENGTH;
    }
    
    in.seekg(length, std::ios_base::cur);
    if (!in.good()) {
        std::cerr << "Skip bytes: Unexpected end of stream." << std::endl;
        return ERR_SKIPB_UNEXPECTED_EOF;
    }
    return length;
}

void tools::hash(const unsigned char *input, size_t input_byte_len, unsigned char *output) {
    // Set the Keccak parameters
    const unsigned int rate                 = 1088;
    const unsigned int capacity             = 512;
    const unsigned char delimited_suffix    = 0x01;
    const size_t output_length              = hash::HASH_SIZE;
    
    // Hash
    Keccak(rate, capacity, input, input_byte_len, delimited_suffix, output, output_length);
}

int tools::tree_hash(const std::vector<unsigned char> & hash_vect, hash::hash_t & root_hash) {
    
    // Check buf_len
    // - each hash is 32 bytes-long => buf_len must be a multiple of 32
    if (hash_vect.size() == 0 || hash_vect.size() % 32 != 0) {
        std::cerr << "TreeHash: Invalid input length." << std::endl;
        return ERR_THASH_INVALID_IN_LEN;
    }
        
    // Calculate the hash count and check it for sanity
    // - bound of 0x10000000 is included in the original Monero function
    size_t hash_count = hash_vect.size() / 32;
    if (hash_count == 0 || hash_count > 0x10000000) {
        std::cerr << "TreeHash: Invalid input length." << std::endl;
        return ERR_THASH_INVALID_IN_LEN;
    }
    
    // Reintepret the hash_vect as array of 32 byte-long hashes
    const unsigned char (* hashes) [hash::HASH_SIZE] = reinterpret_cast<const unsigned char (*)[32]>(hash_vect.data());
    
    switch (hash_count) {
        case 1:
            // Just copy the single hash into the root_hash
            memcpy(&root_hash[0], hashes, root_hash.size());
            return OK;
        case 2:
            // Just hash the two hashes and put the result into root_hash
            hash(hashes[0], 2 * hash::HASH_SIZE, &root_hash[0]);
            return OK;
            
        default:
            // Hashes passed to this function are treated as tree leaves
            
            // How many nodes do we need in the layer above the leaves?
            // => nearest lower power of two to count of leaves
            // we will use this same array as we progress up the tree, using less and less from the reserved nodes
            size_t int_nodes_count = 1;
            while (int_nodes_count * 2 < hash_count) int_nodes_count *= 2;
            
            // Initialize the tree nodes
            unsigned char (*nodes)[hash::HASH_SIZE];
            size_t nodes_size = int_nodes_count * hash::HASH_SIZE;
            std::vector<unsigned char> nodes_vect;
            nodes_vect.resize(nodes_size, 0x00);
            // Reinterpret the data as array of 32 bit arrays
            nodes = reinterpret_cast<unsigned char (*)[32]>(nodes_vect.data());
            
            // We rarely have exactly 2^x leaves (= hashes)
            // => some do not have a partner for hashing in the lowest layer
            // => we just copy these to the layer above
            size_t singles_count = int_nodes_count * 2 - hash_count;
            memcpy(nodes, hashes, singles_count * hash::HASH_SIZE);
            
            // We will proceed and hash the remaining pairs of hashes
            size_t h_src; // points to the two concatenated hashes that will be hashed
            size_t h_dst; // points to where the resulting hash will be stored
            
            // We start after the singles which were copied in the previous step
            // and hash source hashes by two (from h_src)
            // and put the result into the nodes "in layer above" h_dst
            for (h_src = singles_count, h_dst = singles_count;
                 h_dst < int_nodes_count;
                 h_src += 2, h_dst += 1) {
                hash(hashes[h_src], 2 * hash::HASH_SIZE, nodes[h_dst]);
            }
            
            // Next we perform the same thing until we are left with only 2 hashes (nodes)
            // - but now we do it in place in nodes array
            // => in every iteration, we divide int_nodes_count by 2 as we are moving one layer up in the tree
            // - we do that until we are left with just two nodes (= last layer under the root)
            for ( ; int_nodes_count > 2; int_nodes_count /= 2) {
                for (h_src = 0, h_dst = 0;
                     h_dst < int_nodes_count;
                     h_src += 2, ++h_dst) {
                    hash(nodes[h_src], 2 * hash::HASH_SIZE, nodes[h_dst]);
                }
            }
            
            // The last two hashes (= first two fields of ints)
            // are hashed and the result is put into the root hash
            hash(nodes[0], 2 * hash::HASH_SIZE, &root_hash[0]);
            
            return OK;
    }
}
