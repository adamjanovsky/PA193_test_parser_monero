//
//  main.cpp
//  extract_hash
//
//  Created by Kristián Kozák on 04/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>

using std::string;
using std::cout;
using std::endl;

const int HASH_SIZE = 32;
const string VALID_B_FILENAME = "./valid_block.bin";

/**
 Skip a serialized varint in an input stream

 @param fs input file stream
 @return 0 if ok
 */
int read_till_varint_end(std::ifstream & fs) {
    char c;
    fs.read(&c, 1);
    // read from the stream until there was a byte that started with a zero bit
    while (c & 0x80) fs.read(&c, 1);
    return 0;
}

/**
 Extract hash from a block stored in a given file

 @param filename where the block data are stored (without the initial byte determining the size of the block!)
 @param buffer where to store the extracted hash
 @return 0 if ok
 */
char * extract_hash(const string filename, char * buffer) {
    
    // open the file
    std::ifstream in_valid(filename, std::ios::binary);
    
    // skip three serialized varints (major_version, minor_version, timestamp)
    for (int i = 0; i < 3; i++) read_till_varint_end(in_valid);
    
    // read the following hash into the buffer
    in_valid.read(buffer, HASH_SIZE);
    
    return 0;
}

int main(int argc, const char * argv[]) {
    
    // extract the hash from the file
    char hash[HASH_SIZE];
    extract_hash(VALID_B_FILENAME, hash);
    
    // print the hash
    for (int i = 0; i < HASH_SIZE; i++) printf("%02x ", static_cast<unsigned char>(hash[i]));
    cout << endl;
    
    return 0;
}
