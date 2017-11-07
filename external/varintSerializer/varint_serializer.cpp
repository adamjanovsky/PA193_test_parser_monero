//
//  main.cpp
//  micro_ser
//
//  Created by Kristián Kozák on 03/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <bitset>
#include "varint.h" // varint.h is taken from the Monero project without any changes

using std::string;
using std::cout;
using std::endl;

const string TEST_FILE_NAME = "./test.bin";

/**
 Serialize integer to a strem - wrapper around the write_varint in Monero.

 @param val value to serialize
 @param to_stream strem to serialize the file to
 @return 0 if ok
 */
int serialize(unsigned int & val, std::ostream & to_stream) {
    std::ostreambuf_iterator<char> it(to_stream);
    tools::write_varint(it, val);
    return 0;
}

/**
 Deserialize value from a stream - wrapper around read_varint in Monero.

 @param val where to store the serialized value
 @param from_stream strem to read the val from
 @return 0 if ok
 */
int deserialize(unsigned int & val, std::istream & from_stream) {
    typedef std::istreambuf_iterator<char> it;
    tools::read_varint(it(from_stream), it(), val);
    return 0;
}

int main(int argc, const char * argv[]) {
    
    // Serialize an unsigned integer to a file
    unsigned int val = 424242;
    std::ofstream out_f(TEST_FILE_NAME, std::fstream::binary);
    serialize(val, out_f);
    cout << "Serializing integer:   " << val << endl;
    out_f.flush();
    
    // Read it back from the file
    unsigned int val_copy;
    std::ifstream in_f(TEST_FILE_NAME, std::fstream::binary);
    deserialize(val_copy, in_f);
    cout << "Loaded back integer:   " << val_copy << endl;
    
    // Dump contents of the file
    // - notice only the last byte is < 0x80
    cout << "Serialized int in hex: ";
    in_f.seekg(0, out_f.beg);
    char c = 0;
    c = in_f.get();
    while (c != EOF) {
        printf("%02x ", static_cast<unsigned char>(c));
        c = in_f.get();
    }
    cout << endl;
    
    return 0;
}
