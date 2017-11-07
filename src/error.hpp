//
//  error.hpp
//  parser
//
//  Created by Kristián Kozák on 07/11/2017.
//  Copyright © 2017 MUNI. All rights reserved.
//

#ifndef error_h
#define error_h

// OK return val
#define OK   0


// # Tools

// Read varint errors
#define ERR_READV_OVERFLOW          -1
#define ERR_READV_UNEXPECTED_EOF    -2

// Skip varint errors
#define ERR_SKIPV_UNEXPECTED_EOF    -1
#define ERR_SKIPV_VARINT_TOO_LONG   -2

// Skip bytes errors
#define ERR_SKIPB_UNEXPECTED_EOF    -1
#define ERR_SKIPB_INVALID_LENGTH    -2

// Tree hash errors
#define ERR_THASH_INVALID_IN_LEN    -1
#define ERR_THASH_NULL_INPUT        -2
#define ERR_THASH_OUT_OF_MEM        -3


// # Block

// Load header
#define ERR_BL_LOADHDR_INVALID_FORMAT   -1
#define ERR_BL_LOADHDR_OUT_OF_MEM       -2
#define ERR_BL_LOADHDR_EOF              -3


#endif /* error_h */
