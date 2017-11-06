//
//  hash.hpp
//  tree_hash_clear
//
//  Created by Kristián Kozák on 06/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#ifndef hash_hpp
#define hash_hpp

#include <stdio.h>
#include "config.h"

namespace hash {
    /**
     Executes Keccak hash function with parameters extracted from monero.
     
     @param input input to the hash function
     @param input_byte_len length of the input
     @param output where to store the output (32 bytes, needs to be allocated beforehand!)
     */
    void hash(const char *input, size_t input_byte_len, char *output);
    
    /**
     Calculate root hash of a Merkle tree with the leaves being hashes of transactions.
     
     @param hash_buffer concatenated hashes of transactions (32 bytes each hash)
     @param buf_len length of the hash buffer (=> must be a multiple of 32)
     @param root_hash where to store the root hash (32 bytes, needs to be allocated beforehand!)
     */
    void tree_hash(const char *hash_buffer, size_t buf_len, char *root_hash);
}

#endif /* hash_hpp */
