# CMakeLists.txt para compilar con optimización máxima
cmake_minimum_required(VERSION 3.12)
project(boolean_framework)

find_package(pybind11 REQUIRED)
find_package(OpenMP REQUIRED)

# Motor principal
pybind11_add_module(boolean_core boolean_core.cpp)
target_compile_options(boolean_core PRIVATE -O3 -march=native)
target_link_libraries(boolean_core PRIVATE OpenMP::OpenMP_CXX)

# Funciones de grafos
pybind11_add_module(graph_functions graph_functions.cpp)
target_compile_options(graph_functions PRIVATE -O3 -march=native)
target_link_libraries(graph_functions PRIVATE OpenMP::OpenMP_CXX)