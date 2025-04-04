CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Iutils

SRC = main.cpp src/Ranker.cpp src/Trees.cpp src/Node.cpp
TARGET = rankers

build: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
