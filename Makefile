SRC = src/main.cpp
TARGET = bin/parser
TEST_DIR = test

$(TARGET): $(SRC)
	g++ -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
	make clean -C $(TEST_DIR)

test:
	make -C $(TEST_DIR)
	# Run tests
	$(TEST_DIR)/testable_test
	$(TEST_DIR)/hash_test
	# Clean up
	make clean -C $(TEST_DIR)

check:
	cppcheck --enable=all --inconclusive --std=posix src

.PHONY: clean test
