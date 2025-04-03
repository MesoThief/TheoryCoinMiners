CXX = g++
CXXFLAGS = -std=c++17 -Iinclude

SRC = $(shell find src -name "*.cpp")
TARGET = main

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
