# All cpp sources, keccak will be compiled separately
SRC     = main.cpp src/*.cpp

OBJECTS = main.o parser.o block.o tools.o
TARGET = bin/parser
TEST_DIR = test

$(TARGET): $(OBJECTS) keccak.o
	g++ $(OBJECTS) keccak.o -o $(TARGET)
	# clean up the object files
	rm -f *.o

$(OBJECTS): $(SRC) src/*.hpp
	g++ -c -std=c++11 $(SRC)

keccak.o: src/keccak/*
	gcc -c src/keccak/Keccak-readable-and-compact.c -o keccak.o

clean:
	rm -f $(TARGET)
	make clean -C $(TEST_DIR)

test:
	make -C $(TEST_DIR)
	# Run tests
	$(TEST_DIR)/testable_test
	$(TEST_DIR)/tools_test
	# Clean up
	make clean -C $(TEST_DIR)

check:
	cppcheck --enable=all --inconclusive --std=posix src

.PHONY: clean test
