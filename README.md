![Travis CI build info](https://travis-ci.org/adamjanovsky/PA193_test_parser_monero.svg?branch=master)

# Monero block parser

## Scope

The document specifies the program that was created as a project to PA193 at FI MUNI in autumn 2017. The purpose of the program is to parse the block and verify the validity of the previous block.

## Introduction

The input for the program are two binary files with two consecutive blocks. First binary file shall contain the block presented higher in the blockchain, thus containing hash of the previous block. The program reads the hash of the first block. Further the program parses the second block and computes the corresponding hash. The program compares both blocks and prints the result to the standard output.

## How it works

The following list describes the actions program takes in order to verify the validity of blocks. 

### Higher block

0. Note that the program assumes validity of this block, e.g. if the block is invalid and wrong hash is read, the lower block is claimed invalid as well (though possibly being valid)
1. The program parses varints contained in the header until it reaches the hash
2. The program reads 32 bytes of the hash
3. The program stores the hash into variable denoted as `higherBlockHash`

### Lower block

1. The goal of this part is to process the block binary data (blob) the same way monero does before hashing it
2. First, read the varints from block header and store the original bytes as byte array, denote the array `BlockHeader`
3. Then parse the miner_tx and store it as bytes
4. Compute hash of miner_tx and store it in vector of hashes
5. Append hashes of all other transactions contained in the block to the vector and store the count of hashes, denoted `numOfHashes`
6. Compute tree root hash of the vector (Merkle tree root hash), denote the hash `treeRootHash`
7. create binary array `toHash = blockHeader || numOfHashes || treeRootHash` where `||` denotes concatenation
8. Hash the array into the result `lowerBlockHash`

In the end the program checks whether `higherBlockHash == lowerBlockHash` and prints the result to the standard output.

## Notes on Testing

- unit tests are located in `test/unit_tests`
- to build and run the tests, run `make test` in the _root_ of the project
- there is a Makefile in `test` dir as well
  - but no need to use it directly (it is called from the main Makefile)
  - this one just needs to be editted when a new test is created
