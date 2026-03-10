// boolean_core.cpp
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <vector>
#include <cstdint>
#include <omp.h>  // OpenMP para paralelización

namespace py = pybind11;

// Evaluador ultra-eficiente
py::array_t<bool> compute_truth_table_parallel(std::function<bool(uint64_t)> func, int n_bits, int num_threads = -1) {
    uint64_t size = 1ULL << n_bits;
    
    // Alojar memoria
    auto result = py::array_t<bool>(size);
    auto buf = result.request();
    bool* ptr = static_cast<bool*>(buf.ptr);
    
    // Configurar threads
    if (num_threads > 0) {
        omp_set_num_threads(num_threads);
    }
    
    // Paralelizar evaluaciones (crítico para funciones pesadas)
    #pragma omp parallel for schedule(dynamic)
    for (uint64_t i = 0; i < size; ++i) {
        ptr[i] = func(i);
    }
    
    return result;
}

// Versión con callback de progreso
py::array_t<bool> compute_with_progress(
    std::function<bool(uint64_t)> func,
    int n_bits,
    std::function<void(double)> progress_callback,
    int num_threads = -1)
{
    uint64_t size = 1ULL << n_bits;
    auto result = py::array_t<bool>(size);
    auto buf = result.request();
    bool* ptr = static_cast<bool*>(buf.ptr);
    
    if (num_threads > 0) {
        omp_set_num_threads(num_threads);
    }
    
    std::atomic<uint64_t> completed{0};
    
    #pragma omp parallel for schedule(dynamic)
    for (uint64_t i = 0; i < size; ++i) {
        ptr[i] = func(i);
        
        // Reportar progreso cada 1000 evaluaciones
        if (++completed % 1000 == 0) {
            progress_callback(static_cast<double>(completed) / size);
        }
    }
    
    return result;
}

PYBIND11_MODULE(boolean_core, m) {
    m.def("compute_truth_table", &compute_truth_table_parallel,
          py::arg("func"), py::arg("n_bits"), py::arg("num_threads") = -1);
    
    m.def("compute_with_progress", &compute_with_progress,
          py::arg("func"), py::arg("n_bits"), 
          py::arg("progress_callback"),
          py::arg("num_threads") = -1);
}