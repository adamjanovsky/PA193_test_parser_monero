//
//  config.hpp
//  tree_hash_clear
//
//  Created by Kristián Kozák on 06/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#ifndef config_hpp
#define config_hpp

#include <array>


namespace hash {
    // Length of a hash in bytes
    const int HASH_SIZE = 32;
    
    // Type for storing has values
    typedef std::array<unsigned char, HASH_SIZE> hash_t;
}

#endif /* config_h */