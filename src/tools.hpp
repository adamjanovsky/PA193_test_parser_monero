//
//  hash.hpp
//  tree_hash_clear
//
//  Created by Kristián Kozák on 06/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#ifndef tools_hpp
#define tools_hpp

#include <stdio.h>
#include "hash.hpp"
#include <istream>
#include <vector>
#include <array>

namespace tools {
    
    /**
     * @brief - Read a byte in the stream and check that it matches the expected value.
     * @param in - stream to read from
     * @param expected_byte - expected value of the next bute
     * @return - 0 if ok, neg otherwise
     */
    int expect_byte(std::istream & in, unsigned char expected_byte);
    
    /**
     * @brief - Serialize varint to a vector (append the bytes)
     * @param out_vect - push back the bytes to this vect
     * @param val - value to serialize
     * @return - 0 if ok, negative otherwise
     */
    int write_varint(std::vector<unsigned char> & out_vect, unsigned long val);
    
    /**
     * @brief - Read a serialized varint from a stream
     * @param in - stream to read the varint from
     * @param val - where to save the varint
     * @return - 0 if ok, negative otherwise
     */
    int read_varint(std::istream & in, unsigned long & val);
    
    /**
     * @brief - Skips a serialized varint in the input stream.
     * @param in - stream to read from
     * @return - number of bytes skipped if ok, negative val if error
     */
    int skip_varint(std::istream & in);
    
    /**
     * @brief - Skips a desired amount of bytes in stream and checks whether the stream did not end in the process.
     * @param in - stream to skip the bytes in
     * @param - length how many bytes to skip
     * @return - length if ok, negative val if error
     */
    int skip_bytes(std::istream & in, unsigned int length);
    
    /**
     * @brief - Executes Keccak hash function with parameters extracted from monero.
     * @param input - input to the hash function
     * @param input_byte_len - length of the input
     * @param output - where to store the output (32 bytes, needs to be allocated beforehand!)
     */
    void hash(const unsigned char *input, size_t input_byte_len, char unsigned *output);
    
    /**
     * @brief - Calculate root hash of a Merkle tree with the leaves being hashes of transactions.
     * @param hash_vect - concatenated hashes of transactions (32 bytes each hash)
     * @param root_hash - where to store the root hash
     */
    int tree_hash(const std::vector<unsigned char> & hash_vect, hash::hash_t & root_hash);
}

#endif /* tools_hpp */
