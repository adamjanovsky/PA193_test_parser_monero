# Test Monero Parser

Monero cryptocurrency parser. The repository is a product of PA193 seminar on FI MUNI. The purpose of the code is to check whether given Monero block is valid, as well as to check the validity of the transcations within.

## Notes on Testing

- unit tests are located in `test/unit_tests`
- to build and run the tests, run `make test` in the _root_ of the project
- there is a Makefile in `test` dir as well
  - but no need to use it directly (it is called from the main Makefile)
  - this one just needs to be editted when a new test is created
