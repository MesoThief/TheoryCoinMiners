CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -O3 -march=native -flto -DNDEBUG -pthread
BIN_DIR = bin

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG
endif

NOCP ?= 0
ifeq ($(NOCP), 1)
	CXXFLAGS += -DNOCP
endif

MAIN = main
SIMON_TREE = simon_tree
SHORTLEX = shortlex
XY_TREE = xy_tree
MATCH_SIM_K = match_sim_k
RT_EXP = rt_exp
RT_EXP_NOCP = rt_exp_nocp
MATCH_SIM_K_RT_ANALYSIS = match_sim_k_rt_analysis
MATCH_SIM_K_RT_ANALYSIS_NOCP = match_sim_k_rt_analysis_nocp

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
	
$(MATCH_SIM_K): src/match_sim_k.cpp $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< $(SRC) -o $(BIN_DIR)/$@
	
$(RT_EXP): src/rt_exp.cpp $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< $(SRC) -o $(BIN_DIR)/$@

$(RT_EXP_NOCP): src/rt_exp.cpp $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< $(SRC) -o $(BIN_DIR)/$@

$(MATCH_SIM_K_RT_ANALYSIS): src/match_sim_k_rt_analysis.cpp $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< $(SRC) -o $(BIN_DIR)/$@

$(MATCH_SIM_K_RT_ANALYSIS_NOCP): src/match_sim_k_rt_analysis.cpp $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -DNOCP $< $(SRC) -o $(BIN_DIR)/$@

all: $(MAIN) $(SIMON_TREE) $(SHORTLEX) $(XY_TREE) $(MATCH_SIM_K)

clean:
	rm -rf $(BIN_DIR)
