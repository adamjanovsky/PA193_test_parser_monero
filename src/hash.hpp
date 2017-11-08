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


const int HASH_SIZE = 32;

namespace hash {
    // Length of a hash in bytes

    typedef std::array<unsigned char, HASH_SIZE> hash;
}

#endif /* config_h */
