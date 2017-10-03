SRC := src/main.cpp
TARGET := bin/parser

$(TARGET): $(SRC)
	g++ -o $(TARGET) $(SRC)

.PHONY: clean
clean:
	rm $(TARGET)

