//
//  hash_test.cpp
//  parser
//
//  Created by Kristián Kozák on 06/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#include "tools.hpp"
#include "gtest/gtest.h"
#include "error.hpp"
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <vector>

namespace {
    
    // Expect byte
    TEST(ExpectByte, HandlesCorrectInput) {
        std::stringstream ss;
        ss  << static_cast<unsigned char>(0x05);
        
        ASSERT_EQ(OK, tools::expect_byte(ss, 0x05));
    }
    
    TEST(ExpectByte, HandlesIncorrectInput) {
        std::stringstream ss;
        ss  << static_cast<unsigned char>(0x05);
        
        ASSERT_EQ(ERR_EXPBT_DOES_NOT_MATCH, tools::expect_byte(ss, 0x01));
    }
    
    TEST(ExpectByte, ExitsOnEOF) {
        std::stringstream ss;
        
        ASSERT_EQ(ERR_EXPBT_UNEXPECTED_EOF, tools::expect_byte(ss, 0x05));
    }
    
    // Write varint
    // expected values are taken from original Monero code.

    TEST(WriteVarint, HandlesLenghtOne) {
        std::vector<unsigned char>vect;
        std::vector<unsigned char>vect_expected;

        // init expected result
        vect_expected.push_back(0x05);
        
        // calculate actual result
        tools::write_varint(vect, 5);
        
        // compare the vects
        ASSERT_EQ(vect.size(), vect_expected.size());
        for (size_t i = 0; i < vect_expected.size(); i++) {
            ASSERT_EQ(vect_expected[i], vect[i]);
        }
    }
    
    TEST(WriteVarint, HandlesLenghtTwo) {
        std::vector<unsigned char>vect;
        std::vector<unsigned char>vect_expected;
        
        // init expected result
        vect_expected.push_back(0x90);
        vect_expected.push_back(0x03);
        
        // calculate actual result
        tools::write_varint(vect, 400);
        
        // compare the vects
        ASSERT_EQ(vect.size(), vect_expected.size());
        for (size_t i = 0; i < vect_expected.size(); i++) {
            ASSERT_EQ(vect_expected[i], vect[i]);
        }
    }
    
    TEST(WriteVarint, HandlesLargeNum) {
        std::vector<unsigned char>vect;
        std::vector<unsigned char>vect_expected;
        
        // init expected result
        vect_expected.push_back(0x80);
        vect_expected.push_back(0x80);
        vect_expected.push_back(0xe0);
        vect_expected.push_back(0xec);
        vect_expected.push_back(0x99);
        vect_expected.push_back(0xe8);
        vect_expected.push_back(0xbd);
        vect_expected.push_back(0xd6);
        vect_expected.push_back(0x7f);

        // calculate actual result
        tools::write_varint(vect, 9200000000000000000);
        
        // compare the vects
        ASSERT_EQ(vect.size(), vect_expected.size());
        for (size_t i = 0; i < vect_expected.size(); i++) {
            ASSERT_EQ(vect_expected[i], vect[i]);
        }
    }
    
    // Read varint
    // expected values are taken from original Monero code.
    
    TEST(ReadVarint, HandlesLenghtOne) {
        std::stringstream ss;
        ss  << static_cast<unsigned char>(0x05);
        unsigned long val;
        tools::read_varint(ss, val);
        
        ASSERT_EQ(5, val);
    }
    
    TEST(ReadVarint, HandlesLenghtTwo) {
        std::stringstream ss;
        ss  << static_cast<unsigned char>(0x90);
        ss  << static_cast<unsigned char>(0x03);
        unsigned long val;
        tools::read_varint(ss, val);
        
        ASSERT_EQ(400, val);
    }
    
    TEST(ReadVarint, HandlesLargeNum) {
        std::stringstream ss;
        ss  << static_cast<unsigned char>(0x80);
        ss  << static_cast<unsigned char>(0x80);
        ss  << static_cast<unsigned char>(0xe0);
        ss  << static_cast<unsigned char>(0xec);
        ss  << static_cast<unsigned char>(0x99);
        ss  << static_cast<unsigned char>(0xe8);
        ss  << static_cast<unsigned char>(0xbd);
        ss  << static_cast<unsigned char>(0xd6);
        ss  << static_cast<unsigned char>(0x7f);
        unsigned long val;
        tools::read_varint(ss, val);
        
        ASSERT_EQ(9200000000000000000, val);
    }
    
    TEST(ReadVarint, ExitsOnLenghtTenOverflow) {
        std::stringstream ss;
        for (int i = 0; i < 9; i++) {
            ss  << static_cast<unsigned char>(0x81);
        }
        ss  << static_cast<unsigned char>(0x02);
        unsigned long val;

        ASSERT_EQ(ERR_READV_OVERFLOW, tools::read_varint(ss, val));
    }
    
    TEST(ReadVarint, ExitsOnExcessiveLength) {
        std::stringstream ss;
        for (int i = 0; i < 11; i++) {
            ss  << static_cast<unsigned char>(0x82);
        }
        unsigned long val;

        ASSERT_EQ(ERR_READV_OVERFLOW, tools::read_varint(ss, val));
    }
    
    TEST(ReadVarint, ExitsOnEOF) {
        std::stringstream ss;
        ss  << static_cast<unsigned char>(0x82);
        ss  << static_cast<unsigned char>(0x82);
        unsigned long val;

        ASSERT_EQ(ERR_READV_UNEXPECTED_EOF, tools::read_varint(ss, val));
    }
    
    // Skip Varint
    
    TEST(SkipVarint, HandlesLenghtOne) {
        std::stringstream ss;
        ss  << static_cast<unsigned char>(0x01);
        
        ASSERT_EQ(1, tools::skip_varint(ss));
    }
    
    TEST(SkipVarint, HandlesLenghtTwo) {
        std::stringstream ss;
        ss  << static_cast<unsigned char>(0x82);
        ss  << static_cast<unsigned char>(0x02);

        ASSERT_EQ(2, tools::skip_varint(ss));
    }
    
    TEST(SkipVarint, HandlesLenghtTen) {
        std::stringstream ss;
        for (int i = 0; i < 9; i++) {
            ss  << static_cast<unsigned char>(0x81);
        }
        ss  << static_cast<unsigned char>(0x01);
        
        ASSERT_EQ(10, tools::skip_varint(ss));
    }
    
    TEST(SkipVarint, ExitsOnExcessiveLength) {
        std::stringstream ss;
        for (int i = 0; i < 11; i++) {
            ss  << static_cast<unsigned char>(0x82);
        }
        
        ASSERT_EQ(ERR_SKIPV_VARINT_TOO_LONG, tools::skip_varint(ss));
    }
    
    TEST(SkipVarint, ExitsOnEOF) {
        std::stringstream ss;
        ss  << static_cast<unsigned char>(0x82);
        ss  << static_cast<unsigned char>(0x82);
        
        ASSERT_EQ(ERR_SKIPV_UNEXPECTED_EOF, tools::skip_varint(ss));
    }
    
    
    // Skip bytes
    
    TEST(SkipBytes, ReturnsSkippedLength) {
        std::stringstream ss;
        ss  << static_cast<unsigned char>(0x82);
        ss  << static_cast<unsigned char>(0x82);
        ss  << static_cast<unsigned char>(0x82);

        ASSERT_EQ(2, tools::skip_bytes(ss, 2));
    }
    
    TEST(SkipBytes, SkipsTillStreamEnd) {
        std::stringstream ss;
        ss  << static_cast<unsigned char>(0x82);
        ss  << static_cast<unsigned char>(0x82);
        
        ASSERT_EQ(2, tools::skip_bytes(ss, 2));
    }
    
    TEST(SkipBytes, ExitsOnEOF) {
        std::stringstream ss;
        ss  << static_cast<unsigned char>(0x82);
        ss  << static_cast<unsigned char>(0x82);
        
        ASSERT_EQ(ERR_SKIPB_UNEXPECTED_EOF, tools::skip_bytes(ss, 3));
    }
    
    TEST(SkipBytes, ExitsOnZeroLength) {
        std::stringstream ss;
        
        ASSERT_EQ(ERR_SKIPB_INVALID_LENGTH, tools::skip_bytes(ss, 0));
    }
    
    // Tree hash
    
    TEST(TreeHash, HandlesLengthOne) {
        // Source
        const size_t source_len = HASH_SIZE;
        unsigned char source[source_len];
        
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
        unsigned char result[HASH_SIZE];
        tools::tree_hash(source, source_len, result);
        
        // Compare
        ASSERT_EQ(0, memcmp(result_expected, result, HASH_SIZE));
    }
    
    TEST(TreeHash, HandlesLenghtTwo) {
        
        // Source
        const size_t source_len = HASH_SIZE * 2;
        unsigned char source[source_len];
        
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
        unsigned char result[HASH_SIZE];
        tools::tree_hash(source, source_len, result);
        
        // Compare
        ASSERT_EQ(0, memcmp(result_expected, result, HASH_SIZE));
    }
    
    TEST(TreeHash, HandlesLengthPowerOfTwo) {
        
        // Source
        const size_t source_len = HASH_SIZE * 32;
        unsigned char source[source_len];
        
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
        unsigned char result[HASH_SIZE];
        tools::tree_hash(source, source_len, result);
        
        // Compare
        ASSERT_EQ(0, memcmp(result_expected, result, HASH_SIZE));
    }
    
    TEST(TreeHash, HandlesLengthNotPowerOfTwo) {
        
        // Source
        const size_t source_len = HASH_SIZE * 33;
        unsigned char source[source_len];
        
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
        unsigned char result[HASH_SIZE];
        tools::tree_hash(source, source_len, result);
        
        // Compare
        ASSERT_EQ(0, memcmp(result_expected, result, HASH_SIZE));
    }
    
    TEST(TreeHash, DiesOnNullInputBuffer) {
        unsigned char * source = NULL;
        unsigned char result[32];
        size_t length = 1;
        
        ASSERT_EQ(ERR_THASH_NULL_INPUT, tools::tree_hash(source, length, result));
    }
    
    TEST(TreeHash, DiesOnNullOutputBuffer) {
        unsigned char source[32];
        unsigned char * result = NULL;
        size_t length = 1;
        
        ASSERT_EQ(ERR_THASH_NULL_INPUT, tools::tree_hash(source, length, result));
    }
    
    TEST(TreeHash, DiesOnNegativeLength) {
        unsigned char source[32];
        unsigned char result[32];
        size_t length = -1;

        ASSERT_EQ(ERR_THASH_INVALID_IN_LEN, tools::tree_hash(source, length, result));
    }
    
    TEST(TreeHash, DiesOnZeroLength) {
        unsigned char source[32];
        unsigned char result[32];
        size_t length = 0;
        
        ASSERT_EQ(ERR_THASH_INVALID_IN_LEN, tools::tree_hash(source, length, result));
    }
    
    TEST(TreeHash, DiesOnLengthNotDivisibleBy32) {
        unsigned char source[32];
        unsigned char result[32];
        size_t length = 5;
        
        ASSERT_EQ(ERR_THASH_INVALID_IN_LEN, tools::tree_hash(source, length, result));
    }
    
    TEST(TreeHash, DiesOnExcessiveLength) {
        unsigned char source[32];
        unsigned char result[32];
        size_t length = 0x10000001; // 0x10000000 is the upper bound in Monero project
        
        ASSERT_EQ(ERR_THASH_INVALID_IN_LEN, tools::tree_hash(source, length, result));
    }
    
}  // namespace
