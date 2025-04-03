CXX = g++
CXXFLAGS = -std=c++17 -Iinclude

SRC = src/*.cpp
TARGET = main

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
