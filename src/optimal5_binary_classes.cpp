#include "optimal5/optimal5.h"
#include <vector>
using namespace std;

const uint64_t LAST_FUNC = 4294967295;
const int NUM_CLASSES = 16;
const int BUFFER_SIZE = 10000;

int main(int argc, char* argv[]) {
    FILE* files[16];
    uint32_t buffers[NUM_CLASSES][BUFFER_SIZE];
    int counters[NUM_CLASSES];
    opt5::Optimiser opt("third_party/optimal5/knuthies.dat");

    // Abrimos los 16 archivos de cada clase
    for (int i = 0; i < NUM_CLASSES; i++) {
        std::string filename = "data/optimal5/data_" + std::to_string(i) + ".bin";
        counters[i] = 0;
        files[i] = fopen(filename.c_str(), "wb");
    }

    for(uint64_t func = 0; func <= LAST_FUNC; func++) {
        // Calculamos el número mínimo de puertas
        auto gv = opt.lookup(func);
        uint32_t num_gates = gv.n_gates;

        // Guardamos en el buffer de su número de puertas
        buffers[num_gates][counters[num_gates]] = func;
        counters[num_gates]++;

        // Si el buffer está lleno, vaciamos su contenido en el fichero de su número de puertas
        if (counters[num_gates] == BUFFER_SIZE) {
            int n_written = fwrite(buffers[num_gates], sizeof(uint32_t), BUFFER_SIZE, files[num_gates]);
            counters[num_gates] = 0;

            if (n_written != BUFFER_SIZE) {
                cerr << "Failure while writing buffer";
                return 1;
            }
        }

        if ((func & 0xffffff) == 0xffffff)
            std::cerr << (func + 1) << " chains completed." << std::endl;
    }

    for (int i = 0; i < NUM_CLASSES; ++i) {
        // Si falta algo por escribir en el buffer, lo escribimos
        if (counters[i] > 0) {
            int n_written = fwrite(buffers[i], sizeof(uint32_t), counters[i], files[i]);
            if (counters[i] != n_written)
                std::cerr << "Failure while flushing buffer " << i;
            counters[i] = 0;
        }
        // Cerramos el archivo
        fclose(files[i]);
    }

    return 0;
}
