//
//  hash_test.cpp
//  parser
//
//  Created by Kristián Kozák on 06/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#include "tools.hpp"
#include "gtest/gtest.h"
#include <cstring>
#include <cstdlib>

namespace {
    TEST(TreeHash, HandlesLengthOne) {
        // Source
        const size_t source_len = HASH_SIZE;
        char source[source_len];
        
        // All bits set to 0x00
        memset(source, 0x00, source_len);
        
        // Expected result - produced by the original Monero code
        unsigned char result_expected [] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        
        // Calculate our result
        char result[HASH_SIZE];
        tools::tree_hash(source, source_len, result);
        
        // Compare
        ASSERT_EQ(0, memcmp(result_expected, result, HASH_SIZE));
    }
    
    TEST(TreeHash, HandlesLenghtTwo) {
        
        // Source
        const size_t source_len = HASH_SIZE * 2;
        char source[source_len];
        
        // All bits set to 0x01
        memset(source, 0x01, source_len);
        
        // Expected result - produced by the original Monero code
        unsigned char result_expected [] = {
            0x40, 0x16, 0x17, 0xbc, 0x4f, 0x76, 0x93, 0x81,
            0xf8, 0x6b, 0xe4, 0x0d, 0xf0, 0x20, 0x7a, 0x0a,
            0x3e, 0x31, 0xae, 0x08, 0x39, 0x49, 0x7a, 0x5a,
            0xc6, 0xd4, 0x25, 0x2d, 0xfc, 0x35, 0x57, 0x7f
        };
        
        // Calculate our result
        char result[HASH_SIZE];
        tools::tree_hash(source, source_len, result);
        
        // Compare
        ASSERT_EQ(0, memcmp(result_expected, result, HASH_SIZE));
    }
    
    TEST(TreeHash, HandlesLengthPowerOfTwo) {
        
        // Source
        const size_t source_len = HASH_SIZE * 32;
        char source[source_len];
        
        // All bytes set to 0x02
        memset(source, 0x02, source_len);
        
        // Expected result - produced by the original Monero code
        unsigned char result_expected [] = {
            0xb4, 0x3c, 0x04, 0xff, 0x16, 0x08, 0x76, 0x85,
            0x3d, 0x95, 0xb0, 0xe5, 0xdf, 0x22, 0xaf, 0xd1,
            0x64, 0x9a, 0x07, 0xdc, 0x36, 0x53, 0x7a, 0xe7,
            0xc6, 0x22, 0x85, 0xe5, 0x9f, 0x9b, 0x99, 0x03,
        };
        
        // Calculate our result
        char result[HASH_SIZE];
        tools::tree_hash(source, source_len, result);
        
        // Compare
        ASSERT_EQ(0, memcmp(result_expected, result, HASH_SIZE));
    }
    
    TEST(TreeHash, HandlesLengthNotPowerOfTwo) {
        
        // Source
        const size_t source_len = HASH_SIZE * 33;
        char source[source_len];
        
        // All bytes set to 0x03
        memset(source, 0x03, source_len);
        
        // Expected result - produced by the original Monero code
        unsigned char result_expected [] = {
            0xad, 0x02, 0x33, 0x87, 0x53, 0x41, 0x94, 0xc4,
            0x79, 0x1a, 0xbe, 0x28, 0x25, 0x9b, 0x7f, 0xe7,
            0x44, 0x34, 0x5a, 0x5b, 0x94, 0xdd, 0x15, 0xe1,
            0x5c, 0xc2, 0x6f, 0x66, 0x24, 0x66, 0xe7, 0xa9,
        };
        
        // Calculate our result
        char result[HASH_SIZE];
        tools::tree_hash(source, source_len, result);
        
        // Compare
        ASSERT_EQ(0, memcmp(result_expected, result, HASH_SIZE));
    }
    
    TEST(TreeHash, DiesOnNegativeLength) {
        char source[32];
        char result[32];
        size_t length = -1;

        ASSERT_EXIT(tools::tree_hash(source, length, result),
                    ::testing::ExitedWithCode(1),
                    "TreeHash: Invalid input.");
    }
    
    TEST(TreeHash, DiesOnZeroLength) {
        char source[32];
        char result[32];
        size_t length = 0;
        
        ASSERT_EXIT(tools::tree_hash(source, length, result),
                    ::testing::ExitedWithCode(1),
                    "TreeHash: Invalid input.");
    }
    
    TEST(TreeHash, DiesOnLengthNotDivisibleBy32) {
        char source[32];
        char result[32];
        size_t length = 5;
        
        ASSERT_EXIT(tools::tree_hash(source, length, result),
                    ::testing::ExitedWithCode(1),
                    "TreeHash: Invalid input.");
    }
    
    TEST(TreeHash, DiesOnExcessiveLength) {
        char source[32];
        char result[32];
        size_t length = 0x10000001; // 0x10000000 is the upper bound in Monero project
        
        ASSERT_EXIT(tools::tree_hash(source, length, result),
                    ::testing::ExitedWithCode(1),
                    "TreeHash: Invalid input.");
    }
    
}  // namespace
