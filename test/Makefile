CXX = g++
BUILD = test/cpp/bin
SRC_DIR = test/cpp/src

# Flags comunes (igual que el Makefile principal)
FLAGS_BASE = -Ilib -Ithird_party
FLAGS_LIB  = -Ilib/cpp -Ithird_party

# Flags de depuración: actívate con `make DEBUG=1`
ifdef DEBUG
  FLAGS_DEBUG = -g -O0
else
  FLAGS_DEBUG =
endif

# Flags de compilación estática: actívate con `make STATIC=1`
ifdef STATIC
  FLAGS_LINK = -static-libgcc -static-libstdc++ -static -lpthread
else
  FLAGS_LINK = -lpthread
endif

# Librerías comunes (reutiliza del proyecto principal)
LIB_COMMON = lib/cpp/boolean_functions/boolean_functions.cpp \
             lib/cpp/graph_manipulations/graph.cpp

HEADERS_COMMON = lib/cpp/boolean_functions/boolean_functions.h \
                 lib/cpp/graph_manipulations/graph.h

# Lista de todos los ejecutables de test
ALL = $(BUILD)/test_binaries_classes.exe \
      $(BUILD)/test_boolean_functions.exe \
      $(BUILD)/debug_fail.exe

all: $(BUILD) $(ALL)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/test_binaries_classes.exe: $(SRC_DIR)/test_binaries_classes.cpp
	$(CXX) $(FLAGS_LIB) $(FLAGS_DEBUG) $< -o $@ $(FLAGS_LINK)

$(BUILD)/test_boolean_functions.exe: $(SRC_DIR)/test_boolean_functions.cpp $(LIB_COMMON) $(HEADERS_COMMON)
	$(CXX) $(FLAGS_LIB) $(FLAGS_DEBUG) $(filter %.cpp, $^) -o $@ $(FLAGS_LINK)

$(BUILD)/debug_fail.exe: $(SRC_DIR)/debug_fail.cpp $(LIB_COMMON) $(HEADERS_COMMON)
	$(CXX) $(FLAGS_LIB) $(FLAGS_DEBUG) $(filter %.cpp, $^) -o $@ $(FLAGS_LINK)

clean:
	rm -f $(ALL)

.PHONY: all clean