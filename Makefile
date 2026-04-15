CXX = g++
BUILD = build

# Flags comunes
FLAGS_BASE = -Ilib -Ithird_party
FLAGS_LIB  = -Ilib/cpp -Ithird_party
FLAGS_STATIC = -static-libgcc -static-libstdc++ -static -lpthread

# Flags de depuración: actívate con `make DEBUG=1`
ifdef DEBUG
  FLAGS_DEBUG = -g -O0
  FLAGS_STATIC =
else
  FLAGS_DEBUG =
endif

# Librerías comunes
LIB_COMMON = lib/cpp/boolean_functions/boolean_functions.cpp \
             lib/cpp/graph_manipulations/graph.cpp

LIB_FULL = $(LIB_COMMON) \
           lib/cpp/metrics/metrics.cpp \
           lib/cpp/visualization/terminal_formatter.cpp

# Headers — solo para tracking de dependencias, no se compilan directamente
HEADERS_COMMON = lib/cpp/boolean_functions/boolean_functions.h \
                 lib/cpp/graph_manipulations/graph.h

HEADERS_FULL = $(HEADERS_COMMON) \
               lib/cpp/metrics/metrics.h \
               lib/cpp/visualization/terminal_formatter.h

# Lista de todos los ejecutables
ALL = $(BUILD)/optimal5_binary.exe \
      $(BUILD)/optimal5_binary_classes.exe \
      $(BUILD)/optimal5_numgates.exe \
      $(BUILD)/calculate_metric_stochastic_by_classes.exe \
      $(BUILD)/calculate_min_expression.exe \
      $(BUILD)/calculate_fnm_expression.exe

all: $(BUILD) $(ALL)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/main.exe: src/compute_truth_table.cpp $(LIB_COMMON) $(HEADERS_COMMON)
	$(CXX) $(FLAGS_BASE) $(FLAGS_DEBUG) $(filter %.cpp, $^) -o $@

$(BUILD)/optimal5_binary.exe: src/optimal5_binary.cpp
	$(CXX) $(FLAGS_BASE) $(FLAGS_DEBUG) $^ -o $@ $(FLAGS_STATIC)

$(BUILD)/optimal5_binary_classes.exe: src/optimal5_binary_classes.cpp
	$(CXX) $(FLAGS_BASE) $(FLAGS_DEBUG) $^ -o $@ $(FLAGS_STATIC)

$(BUILD)/optimal5_numgates.exe: src/optimal5_numgates.cpp $(LIB_COMMON) $(HEADERS_COMMON)
	$(CXX) $(FLAGS_LIB) $(FLAGS_DEBUG) $(filter %.cpp, $^) -o $@ $(FLAGS_STATIC)

$(BUILD)/calculate_metric_stochastic_by_classes.exe: src/calculate_metric_stochastic_by_classes.cpp $(LIB_FULL) $(HEADERS_FULL)
	$(CXX) $(FLAGS_LIB) $(FLAGS_DEBUG) $(filter %.cpp, $^) -o $@

$(BUILD)/calculate_min_expression.exe: src/calculate_min_expression.cpp $(LIB_FULL) $(HEADERS_FULL)
	$(CXX) $(FLAGS_LIB) $(FLAGS_DEBUG) $(filter %.cpp, $^) -o $@

$(BUILD)/calculate_fnm_expression.exe: src/calculate_fnm_expression.cpp $(LIB_FULL) $(HEADERS_FULL)
	$(CXX) $(FLAGS_LIB) $(FLAGS_DEBUG) $(filter %.cpp, $^) -o $@

clean:
	rm -f $(ALL)
