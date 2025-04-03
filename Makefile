CXX = g++
CXXFLAGS = -std=c++17 -Iinclude
BIN_DIR = bin

MAIN = main
SIMON_TREE = simon_tree
SHORTLEX = shortlex

SRC := $(wildcard src/data/*.cpp src/utils/*.cpp)

all: $(MAIN)

$(MAIN): src/main.cpp $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< $(SRC) -o $(BIN_DIR)/$@

$(SIMON_TREE): src/simon_tree.cpp $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< $(SRC) -o $(BIN_DIR)/$@

$(SHORTLEX): src/shortlex.cpp $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< $(SRC) -o $(BIN_DIR)/$@

clean:
	rm -rf $(BIN_DIR)
