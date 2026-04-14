#include <iostream>
#include <string>
#include <cstdint>
#include "optimal5/optimal5.h"

using namespace std;

int main(int argc, char* argv[]) {
    string FILENAME_PATTERN = "data/optimal5/data_";
    int BUFFER_SIZE = 100000;
    FILE *file;
    opt5::Optimiser opt("third_party/optimal5/knuthies.dat");

    for (int circuit_size = 0; circuit_size < 16; circuit_size++) {
        // Abrir un archivo para lectura en modo binario
        string filename = FILENAME_PATTERN + to_string(circuit_size) + ".bin";
        file = fopen(filename.c_str(), "rb");
        if (file == NULL)
            return 1;
    
        // Aquí podrías leer datos binarios, por ejemplo:
        uint32_t buffer[BUFFER_SIZE];
        size_t num_elements = fread(buffer, sizeof(uint32_t), BUFFER_SIZE, file);
        while(num_elements > 0) {
            for (int i = 0; i < num_elements; i++) {
                uint32_t func = buffer[i];
                auto gv = opt.lookup(func);
                uint32_t num_gates = gv.n_gates;
                if (num_gates != circuit_size) {
                    cerr << "[ERROR] Function " << func << ": expected " << circuit_size << " but " << num_gates << " was looked up\n";
                    return 1;
                }
            }

            num_elements = fread(buffer, sizeof(uint32_t), BUFFER_SIZE, file);
        }
    
        fclose(file);
    }

    return 0;
}
