CXX = g++
CXXFLAGS = -std=c++17 -Iinclude
BIN_DIR = bin

MAIN = main
SIMON_TREE = simon_tree
SHORTLEX = shortlex
XY_TREE = xy_tree

SRC := $(wildcard src/data/*.cpp src/utils/*.cpp)

$(MAIN): src/main.cpp $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< $(SRC) -o $(BIN_DIR)/$@

$(SIMON_TREE): src/simon_tree.cpp $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< $(SRC) -o $(BIN_DIR)/$@

$(SHORTLEX): src/shortlex.cpp $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< $(SRC) -o $(BIN_DIR)/$@

$(XY_TREE): src/xy_tree.cpp $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< $(SRC) -o $(BIN_DIR)/$@

all: $(MAIN) $(SIMON_TREE) $(SHORTLEX) $(XY_TREE)

clean:
	rm -rf $(BIN_DIR)
