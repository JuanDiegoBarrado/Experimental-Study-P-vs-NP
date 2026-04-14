#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <random>
#include <chrono>

#include "boolean_functions/boolean_functions.h"
#include "metrics/metrics.h"
#include "visualization/terminal_formatter.h"

using namespace std;
namespace fs = filesystem;
using ull = unsigned long long;


// ─────────────────────────────────────────────────────────────────────────────
// Global variables
// ─────────────────────────────────────────────────────────────────────────────
const ull MAX_NUM_BINS = 100;
ull BATCH_SIZE = 0;
float PROB = 2.0f;
complexity_metric METRIC;
string METRIC_NAME;
string COMPLEXITY_CLASS = "";

int parse_argument(int argc, char * argv[]);

int main(int argc, char* argv[]) {
    // ── Argument parsing ─────────────────────────────────────────────────────
    if (parse_argument(argc, argv) != 0)
        return -1;

    // ── File paths ──────────────────────────────────────────────────────────
    string functions_data       = "data/optimal5/data_" + COMPLEXITY_CLASS + ".bin";
    string metric_results_path  = "data/" + METRIC_NAME + "/classes/class_" + COMPLEXITY_CLASS + "/";

    // ── Derived constants ────────────────────────────────────────────────────
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    string max_score_path       = metric_results_path + "max_score.txt";
    string min_score_path       = metric_results_path + "min_score.txt";
    string hist_path            = metric_results_path + "histogram.csv";
    string high_path            = metric_results_path + "highest_scored.txt";
    string low_path             = metric_results_path + "lowest_scored.txt";

    error_code ec;
    auto file_sz = fs::file_size(functions_data, ec);
    if (ec) {
        cerr << "Cannot open " << functions_data << ": " << ec.message() << "\n";
        return -1;
    }
    const ull NUM_ELEMS   = ull(file_sz) / 4;
    if (BATCH_SIZE == 0) BATCH_SIZE = NUM_ELEMS;
    const ull NUM_BATCHES = NUM_ELEMS / BATCH_SIZE + (NUM_ELEMS % BATCH_SIZE != 0);
    std::vector<uint32_t> buffer(BATCH_SIZE);
    
    cout    << "--------------------------------------------------------------------------------\n"
            << "Selected class: " << COMPLEXITY_CLASS << "\n"
            << "Number of functions: " << NUM_ELEMS << "\n"
            << "Computed in batches of size " << BATCH_SIZE << ", hence computing " << NUM_BATCHES << " batches\n"
            << "Computed metric: " << METRIC_NAME << "\n"
            << "Probability of testing metric: " << PROB << "\n"
            << "--------------------------------------------------------------------------------\n";

    // ── Pass 1: determine min/max (skipped when fixed values are provided) ──
    cout << "Computing max and min values...\n";

    ull min_val, max_val;
    bool valid = false;
    bool first_batch = true;
    std::ifstream min_score_infile(min_score_path);
    std::ifstream max_score_infile(max_score_path);

    if (min_score_infile.is_open() && max_score_infile.is_open()) {
        // Ambos archivos existen -> leer valores
        min_score_infile >> min_val;
        max_score_infile >> max_val;

        min_score_infile.close();
        max_score_infile.close();
        cout << "Computed max and min values! Time spent: " << format_time(0) << "\n";
    } else {
        // Alguno no existe → calcular valores
        unordered_set<uint32_t> highest_functions;
        unordered_set<uint32_t> lowest_functions;

        ifstream file(functions_data, ios::binary);
        ull nbatch = 0;

        auto start_time = std::chrono::steady_clock::now();
        while (file.read(reinterpret_cast<char*>(buffer.data()), buffer.size() * sizeof(uint32_t)) || file.gcount() > 0) {
            ull num_elements = static_cast<ull>(file.gcount()) / sizeof(uint32_t);

            for (ull i = 0; i < num_elements; ++i) {
                if (dis(gen) > PROB)
                    continue;
                
                truthtable t = to_truthtable(buffer[i]);
                ull v = METRIC(t);
                // if (buffer[i] == 65814) {
                //     cout << v << ": " << print_table(t);
                // }

                if (!valid) {
                    min_val = v; valid = true; lowest_functions.clear();  lowest_functions.insert(buffer[i]);
                    max_val = v; valid = true; highest_functions.clear(); highest_functions.insert(buffer[i]);
                }
                else {
                    if (v == max_val) { highest_functions.insert(buffer[i]); }
                    if (v == min_val) { lowest_functions.insert(buffer[i]); }
                    if (v > max_val)  { max_val = v; valid = true; highest_functions.clear(); highest_functions.insert(buffer[i]); }
                    if (v < min_val)  { min_val = v; valid = true; lowest_functions.clear();  lowest_functions.insert(buffer[i]);  }
                }
            }
            ++nbatch;

            auto now = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed = now - start_time;

            double time_per_batch = elapsed.count() / nbatch;
            double remaining_time = (NUM_BATCHES - nbatch) * time_per_batch;

            if (!first_batch)
                CLEAR_LINES(3)
            cout    << "Done batch no: " << nbatch << ", ETA: " << format_time(remaining_time) << "\n"
                    << "Min function (" << *lowest_functions.begin() << ") of a total of (" << lowest_functions.size() << ") with value (" << min_val << ")\n"
                    << "Max function (" << *highest_functions.begin() << ") of a total of (" << highest_functions.size() << ") with value (" << max_val << ")";
            
            first_batch = false;
        }
        file.close();

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - start_time;
        CLEAR_LINES(3)
        cout    << "Computed max and min values! Time spent: " << format_time(elapsed.count()) << "\n"
                << "--------------------------------------------------------------------------------\n"
                << "Number of functions with highest value (" << max_val << ") of metric: " << highest_functions.size() << "\n"
                << "Sample of up to 10:\n";
        {
            int shown = 0;
            for (uint32_t x : highest_functions) {
                if (shown++ >= 10) break;
                cout << "----> " << x << "\n";
            }
            cout << "\n";
        }
        cout << "Number of functions with lowest value (" << min_val << ") of metric: " << lowest_functions.size() << "\n";
        cout << "Sample of up to 10:\n";
        {
            int shown = 0;
            for (uint32_t x : lowest_functions) {
                if (shown++ >= 10) break;
                cout << "----> " << x << "\n";
            }
            cout << "\n";
        }

        // Save extreme functions and values
        fs::create_directories(metric_results_path, ec);
        {
            ofstream min_score_outfile(min_score_path);
            if (!min_score_outfile) { cerr << "Cannot write " << min_score_path << "\n"; return -1; }
            else min_score_outfile << min_val;
        }
        {
            ofstream max_score_outfile(max_score_path);
            if (!max_score_outfile) { cerr << "Cannot write " << max_score_path << "\n"; return -1; }
            else max_score_outfile << max_val;
        }
        {
            ofstream fh(high_path);        
            if (!fh) { cerr << "Cannot write " << high_path << "\n"; return -1; }
            for (uint32_t x : highest_functions) {
                truthtable t = to_truthtable(x);
                fh << print_table(t) << "\n";
            }
        }
        {
            ofstream fl(low_path);        
            if (!fl) { cerr << "Cannot write " << low_path << "\n"; return -1; }
            for (uint32_t x : lowest_functions) {
                truthtable t = to_truthtable(x);
                fl << print_table(t) << "\n";
            }
        }

        cout << "Highest functions saved in: " << high_path << "\n";
        cout << "Lowest  functions saved in: " << low_path  << "\n";
        cout << "--------------------------------------------------------------------------------\n";
    }
    
    // Build bin edges:  NUM_BINS+1 edges from min_val to max_val  (inclusive)
    int NUM_BINS = min(MAX_NUM_BINS, max_val - min_val + 1);
    vector<double> bin_edges(NUM_BINS + 1);
    double lo = double(min_val);
    double hi = double(max_val);
    double step = (hi - lo) / NUM_BINS;
    for (int b = 0; b <= NUM_BINS; ++b)
        bin_edges[b] = lo + b * step;

    vector<ull> hist_accum(NUM_BINS, 0LL);

    // ── Pass 2: calculate bins ───────────────────────────────────────────────────
    cout << "Computing histogram...\n";
    ifstream file(functions_data, ios::binary);
    ull nbatch = 0;

    auto start_time = std::chrono::steady_clock::now();
    while (file.read(reinterpret_cast<char*>(buffer.data()), buffer.size() * sizeof(uint32_t)) || file.gcount() > 0) {
        int num_elements = static_cast<int>(file.gcount()) / sizeof(uint32_t);

        for (ull i = 0; i < num_elements; ++i) {
            if (dis(gen) > PROB)
                continue;
            
            truthtable t = to_truthtable(buffer[i]);
            ull v = METRIC(t);

            // Bin the value  (same as numpy.histogram with bin_edges)
            double dv = double(v);
            int bin = int((dv - bin_edges[0]) / (bin_edges[NUM_BINS] - bin_edges[0]) * NUM_BINS);
            // Clamp: last edge is inclusive (matches numpy behaviour)
            if (bin < 0)
                bin = 0;
            if (bin >= NUM_BINS)
                bin = NUM_BINS - 1;
            hist_accum[bin]++;
        }
        ++nbatch;

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - start_time;

        double time_per_batch = elapsed.count() / nbatch;
        double remaining_time = (NUM_BATCHES - nbatch) * time_per_batch;

        cout    << CLEAR_LINE
                << "Done batch no: " << nbatch
                << ", ETA: " << format_time(remaining_time);

    }
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = now - start_time;
    cout << CLEAR_LINE << "Computed histogram! Time spent: " << format_time(elapsed.count()) << "\n";
    file.close();

    // ── Save histogram as CSV ────────────────────────────────────────────────
    {
        fs::create_directories(metric_results_path, ec);
        ofstream fh(hist_path);
        if (!fh) {
            cerr << "Cannot write " << hist_path << "\n";
            return -1;
        }

        fh << "bin_left,bin_right,count\n";
        for (int b = 0; b < NUM_BINS; ++b)
            fh << bin_edges[b] << "," << bin_edges[b+1] << "," << hist_accum[b] << "\n";
        cout << "Histogram saved -> " << hist_path << "\n";
    }

    return 0;
}

int parse_argument(int argc, char * argv[]) {
    
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if ((arg == "-b" || arg == "--batch") && i + 1 < argc) {
            BATCH_SIZE = stoull(argv[++i]);
        } else if ((arg == "-p" || arg == "--prob") && i + 1 < argc) {
            PROB = stof(argv[++i]);
        } else if ((arg == "-m" || arg == "--metric") && i + 1 < argc) {
            METRIC_NAME = argv[++i];
            auto it = METRICS.find(METRIC_NAME);
            if (it == METRICS.end()) {
                cerr << "Error: métrica desconocida '" << METRIC_NAME << "'. Disponibles:";
                for (auto& [name, _] : METRICS) cerr << " " << name;
                cerr << "\n";
                return -1;
            }
            METRIC = it->second;
        } else if ((arg == "-c" || arg == "--class") && i + 1 < argc) {
            COMPLEXITY_CLASS = argv[++i];
        } else {
            cerr << "Unknown argument: " << arg << "\n";
            return -1;
        }
    }

    if (METRIC_NAME.empty()) {
        cerr << "Error: -m/--metric is required.\n";
        return -1;
    }
    if (COMPLEXITY_CLASS.empty()) {
        cerr << "Error: -c/--class is required.\n";
        return -1;
    }

    return 0;
}