//
//  hash.cpp
//  tree_hash_clear
//
//  Created by Kristián Kozák on 06/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#include <iostream>
#include "Keccak-readable-and-compact.h"
#include "hash.hpp"
#include "config.h"

void hash::hash(const char *input, size_t input_byte_len, char *output) {
    
    // Cast the input and output as unsigned for the Keccak implementation
    const unsigned char *u_input    = reinterpret_cast<const unsigned char *>(input);
    unsigned char *u_output         = reinterpret_cast<unsigned char *>(output);
    
    // Set the Keccak parameters
    const unsigned int rate                 = 1088;
    const unsigned int capacity             = 512;
    const unsigned char delimited_suffix    = 0x01;
    const size_t output_length              = HASH_SIZE;
    
    // Hash
    Keccak(rate, capacity, u_input, input_byte_len, delimited_suffix, u_output, output_length);
}

void hash::tree_hash(const char * hash_buffer, size_t buf_len, char *root_hash) {
    // Check buf_len
    // - each hash is 32 bytes-long => buf_len must be a multiple of 32
    if (buf_len <= 0 || buf_len % 32 != 0) {
        std::cerr << "TreeHash: Invalid input." << std::endl;
        exit(1);
    }
        
    // Calculate the hash count and check it for sanity
    // - bound of 0x10000000 is included in original Monero function
    size_t hash_count = buf_len / 32;
    if (hash_count <= 0 || hash_count > 0x10000000) {
        printf("TreeHash: Invalid input.");
        exit(1);
    }
    
    // Reintepret the hash_buffer as array of 32 byte-long hashes
    const char (* hashes) [HASH_SIZE] = reinterpret_cast<const char (*)[32]>(hash_buffer);
    
    switch (hash_count) {
        case 1:
            // Just copy the single hash into the root_hash
            memcpy(root_hash, hashes, HASH_SIZE);
            return;
        case 2:
            // Just hash the two hashes and put the result into root_hash
            hash(hashes[0], 2 * HASH_SIZE, root_hash);
            return;
            
        default:
            // Hashes passed to this function are treated as tree leaves
            
            // How many nodes do we need in the layer above the leaves?
            // => nearest lower power of two to count of leaves
            // we will use this same array as we progress up the tree, using less and less from the reserved nodes
            size_t int_nodes_count = 1;
            while (int_nodes_count * 2 < hash_count) int_nodes_count *= 2;
            
            // Initialize the tree nodes
            char (*nodes)[HASH_SIZE];
            size_t nodes_size = int_nodes_count * HASH_SIZE;
            nodes = reinterpret_cast<char (*)[32]>(new(std::nothrow) char[nodes_size]);
            if (nodes == nullptr) {
                printf("TreeHash: Out of memory.");
                exit(1);
            }
            
            memset(nodes, 0, nodes_size);
            
            // We rarely have exactly 2^x leaves (= hashes)
            // => some do not have a partner for hashing in the lowest layer
            // => we just copy these to the layer above
            size_t singles_count = int_nodes_count * 2 - hash_count;
            memcpy(nodes, hashes, singles_count * HASH_SIZE);
            
            // We will proceed and hash the remaining pairs of hashes
            size_t h_src; // points to the two concatenated hashes that will be hashed
            size_t h_dst; // points to where the resulting hash will be stored
            
            // We start after the singles which were copied in the previous step
            // and hash source hashes by two (from h_src)
            // and put the result into the nodes "in layer above" h_dst
            for (h_src = singles_count, h_dst = singles_count;
                 h_dst < int_nodes_count;
                 h_src += 2, h_dst += 1) {
                hash(hashes[h_src], 2 * HASH_SIZE, nodes[h_dst]);
            }
            
            // Next we perform the same thing until we are left with only 2 hashes (nodes)
            // - but now we do it in place in nodes array
            // => in every iteration, we divide int_nodes_count by 2 as we are moving one layer up in the tree
            // - we do that until we are left with just two nodes (= last layer under the root)
            for ( ; int_nodes_count > 2; int_nodes_count /= 2) {
                for (h_src = 0, h_dst = 0;
                     h_dst < int_nodes_count;
                     h_src += 2, ++h_dst) {
                    hash(nodes[h_src], 2 * HASH_SIZE, nodes[h_dst]);
                }
            }
            
            // The last two hashes (= first two fields of ints)
            // are hashed and the result is put into the root hash
            hash(nodes[0], 2 * HASH_SIZE, root_hash);
            
            // Free the nodes array
            delete [] nodes;
            return;
    }
}
