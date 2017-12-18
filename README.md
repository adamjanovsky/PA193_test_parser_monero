![Travis CI build info](https://travis-ci.org/adamjanovsky/PA193_test_parser_monero.svg?branch=master)

# Monero block parser

## Scope

The document specifies the program that was created as a project to PA193 at FI MUNI in autumn 2017. The purpose of the program is to parse the block and verify the validity of the previous block.

## Introduction

The input for the program are two binary files containing two consecutive blocks. First binary file shall contain the block presented lower in the blockchain, thus the block being validated. The second file should contain the higher block in the blockchain, containg the hash of the previous block. The program treats both blocks the same way, parses them into object. Then functions are called on the objects that compare the corresponding hashes (the one computed vs. the one derived from raw block data).

## Compilation

```
git clone https://github.com/adamjanovsky/PA193_test_parser_monero.git
cd PA193_test_parser_monero
make
```

## How it works

The following paragraph describes the actions program takes in order to verify the validity of blocks. The current state of code assumes usage as a command line tool. One provides two command line arguments (lower block filepath and higher block filepath). The program creates object Parser, that creates two blocks. The parser object commands the blocks to parse itself and then the parser can call functions in order to obtain the needed hashes. Finally, the parser compares the hashes and prints the output.

The program can be easily modified to run on as many files as neccessary with single Parser object instance. The parser always clears the blocks as they are parsed. During the testing, all blocks from Monero blockchain were tested. 

## External tools

In directory external, there is a monero blockchain explorer that can be used to understand Monero code and to obtain some sample blocks. Moreover, some block examples are provided as well. Directories blockHeaderParser, hashingBlobExamples and varintSerializier serve only as a proof of concept and there is not guarantee that they work!

## Technical description of the parsing process

The hash of the block contains the following:

1. Whole block header is contained in the hash, denoted `BH`, the block header consists of `minor_version || major_version || timestamp || prev_id || nonce`. All of those, except the `prev_id` are varints. The `prev_id` is the hash of the previous block
2. Miner_tx is parsed into the block. It contains `version || unlock || number of inputs || type of the input transaction || input transactions || num of the output transactions || output transactions || num of extras || extras || (if version 2, then 0x00)`
3. In step 2, several checks are performed. Transaction is discarded if it is not of version 1 or 2. Further, there must be a single input. The input must be of specific type (see code).
4. Then hashes of transactions are added to the hashing contents. Those contain varint specifying number of hashes and then the corresponding hashes, i.e. `num_of_hashes || hash_1 || hash_2 || ... || hash_n`.
5. Block header, miner_tx and transaction hashes are appended together and hashed. Some bytes may be skipped (depending on the version of miner_tx etc...).

## Notes on Testing

- unit tests are located in `test/unit_tests`
- to build and run the tests, run `make test` in the _root_ of the project
- there is a Makefile in `test` dir as well
  - but no need to use it directly (it is called from the main Makefile)
  - this one just needs to be editted when a new test is created
  
 ## Links
 
 - [Output from Travis CI](https://travis-ci.org/adamjanovsky/PA193_test_parser_monero)
