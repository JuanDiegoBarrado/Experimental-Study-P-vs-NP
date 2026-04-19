#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <random>
#include <chrono>
#include <thread>
#include <utility>

#include "boolean_functions/boolean_functions.h"
#include "metrics/metrics.h"
#include "visualization/terminal_formatter.h"

using namespace std;
namespace fs = filesystem;
using ull = uint64_t;

struct ThreadRNG {
    mt19937 gen;
    uniform_real_distribution<float> dist{0.0f, 1.0f};

    ThreadRNG(ull base_seed, unsigned int thread_id) {
        seed_seq seq{
            static_cast<uint32_t>(base_seed),
            static_cast<uint32_t>(base_seed >> 32),
            static_cast<uint32_t>(thread_id),
            static_cast<uint32_t>(thread_id * 0x85EBCA6Bu)
        };
        gen.seed(seq);
    }

    float next() {
        return dist(gen);
    }
};


// ─────────────────────────────────────────────────────────────────────────────
// Global variables
// ─────────────────────────────────────────────────────────────────────────────
const ull MAX_NUM_BINS = 100;
ull BATCH_SIZE = 0;
float PROB = 2.0f;
complexity_metric METRIC;
string METRIC_NAME;
string COMPLEXITY_CLASS = "";
ull RNG_BASE_SEED = 0;
bool RNG_SEED_FROM_CLI = false;

int parse_argument(int argc, char * argv[]);
void print_usage(const char* prog);

int main(int argc, char* argv[]) {
    // ── Argument parsing ─────────────────────────────────────────────────────
    if (parse_argument(argc, argv) != 0)
        return -1;
    // ─────────────────────────────────────────────────────────────────────────

    
    // ── Derived constants ────────────────────────────────────────────────────
    // File paths
    string functions_filepath   = "data/optimal5/data_" + COMPLEXITY_CLASS + ".bin";
    string metric_results_path  = "data/" + METRIC_NAME + "/classes/class_" + COMPLEXITY_CLASS + "/";
    string max_score_path       = metric_results_path + "max_score.txt";
    string min_score_path       = metric_results_path + "min_score.txt";
    string hist_path            = metric_results_path + "histogram.csv";
    string high_path            = metric_results_path + "highest_scored.txt";
    string low_path             = metric_results_path + "lowest_scored.txt";

    // Buffer configuration
    error_code ec;
    auto file_sz = fs::file_size(functions_filepath, ec);
    if (ec) {
        cerr << "Cannot open " << functions_filepath << ": " << ec.message() << "\n";
        return -1;
    }
    const ull NUM_ELEMS = ull(file_sz) / 4;
    BATCH_SIZE = (BATCH_SIZE == 0) ? NUM_ELEMS : BATCH_SIZE;
    const ull NUM_BATCHES = NUM_ELEMS / BATCH_SIZE + (NUM_ELEMS % BATCH_SIZE != 0);
    vector<uint32_t> buffer(BATCH_SIZE);

    // Concurrency configuration
    const unsigned hw_threads = max(1u, thread::hardware_concurrency());
    if (!RNG_SEED_FROM_CLI) {
        random_device rd;
        RNG_BASE_SEED = (static_cast<ull>(rd()) << 32) | static_cast<ull>(rd());
    }
    vector<ThreadRNG> rng_pool;
    rng_pool.reserve(hw_threads);
    for (unsigned tid = 0; tid < hw_threads; ++tid)
        rng_pool.emplace_back(RNG_BASE_SEED, tid);
    
    cout    << "--------------------------------------------------------------------------------\n"
            << "Selected class: " << COMPLEXITY_CLASS << "\n"
            << "Number of functions: " << NUM_ELEMS << "\n"
            << "Computed in batches of size " << BATCH_SIZE << ", hence computing " << NUM_BATCHES << " batches\n"
            << "Computed metric: " << METRIC_NAME << "\n"
            << "Number of threads for concurrency: " << hw_threads << "\n"
            << "RNG base seed: " << RNG_BASE_SEED << (RNG_SEED_FROM_CLI ? " (CLI)" : " (random_device)") << "\n"
            << "Probability of testing metric: " << ((PROB >= 1) ? 1 : PROB) << "\n"
            << "--------------------------------------------------------------------------------" << endl;
    // ─────────────────────────────────────────────────────────────────────────

    // ── Step 1: determine min/max ────────────────────────────────────────────
    cout << "Computing max and min values..." << endl;

    ull min_val, max_val;
    bool valid = false;
    bool first_batch = true;
    ifstream min_score_infile(min_score_path);
    ifstream max_score_infile(max_score_path);

    if (min_score_infile.is_open() && max_score_infile.is_open()) {
        // Both files exists, then read their values
        min_score_infile >> min_val;
        max_score_infile >> max_val;

        min_score_infile.close();
        max_score_infile.close();
        cout << "Computed max and min values! Time spent: " << format_time(0) << endl;
    } else {
        // Any don't exists, then compute min-max values
        unordered_set<uint32_t> highest_functions;
        unordered_set<uint32_t> lowest_functions;
        ifstream file(functions_filepath, ios::binary);
        ull nbatch = 0;

        auto start_time = chrono::steady_clock::now();
        // Read binary file in batches
        while (file.read(reinterpret_cast<char*>(buffer.data()), buffer.size() * sizeof(uint32_t)) || file.gcount() > 0) {
            ull num_elements = static_cast<ull>(file.gcount()) / sizeof(uint32_t);

            if (num_elements <= 0) {
                cerr << "Some bytes readed from " << functions_filepath << ", but not a whole element. Probably the file is corrupted" << endl;
                return -1;
            }

            struct LocalExtrema {
                bool valid = false;
                ull min_v = 0;
                ull max_v = 0;
                unordered_set<uint32_t> lowest;
                unordered_set<uint32_t> highest;
            };

            // Divide the processing within a batch among num_threads to speed up the computation
            unsigned num_threads = static_cast<unsigned>(min<ull>(hw_threads, num_elements));
            ull thread_chunk = (num_elements + num_threads - 1) / num_threads;
            vector<LocalExtrema> local_extrema(num_threads);
            vector<thread> threads;
            threads.reserve(num_threads);

            for (unsigned thread_id = 0; thread_id < num_threads; ++thread_id) {
                // Compute its range
                ull start = thread_id * thread_chunk;
                ull end = min(num_elements, start + thread_chunk);

                // Build the procedure to process its chunk
                threads.emplace_back([&, thread_id, start, end]() {
                    LocalExtrema& ext = local_extrema[thread_id];
                    ThreadRNG& random_gen = rng_pool[thread_id];
                    for (ull i = start; i < end; ++i) {
                        if (random_gen.next() > PROB)
                            continue;

                        truthtable t = to_truthtable(buffer[i]);
                        ull v = METRIC(t);

                        if (!ext.valid) {
                            ext.valid = true;
                            ext.min_v = v;
                            ext.max_v = v;
                            ext.lowest.insert(buffer[i]);
                            ext.highest.insert(buffer[i]);
                            continue;
                        }

                        if (v == ext.max_v) { ext.highest.insert(buffer[i]); }
                        if (v == ext.min_v) { ext.lowest.insert(buffer[i]); }
                        if (v > ext.max_v)  { ext.max_v = v; ext.highest.clear(); ext.highest.insert(buffer[i]); }
                        if (v < ext.min_v)  { ext.min_v = v; ext.lowest.clear();  ext.lowest.insert(buffer[i]);  }
                    }
                });
            }

            // Start each thread and wait until all have finished
            for (thread& worker : threads)
                worker.join();

            // Rebuild global computation based on local computations
            bool batch_valid = false;
            ull batch_min = 0;
            ull batch_max = 0;
            unordered_set<uint32_t> batch_lowest;
            unordered_set<uint32_t> batch_highest;

            // Get min-max data for the whole batch
            for (auto& ext : local_extrema) {
                // If data is not valid, continue
                if (!ext.valid) continue;

                // If there is no current min and max for the whole batch, initialize them
                if (!batch_valid) {
                    batch_valid = true;
                    batch_min = ext.min_v;
                    batch_max = ext.max_v;
                    batch_lowest = move(ext.lowest);
                    batch_highest = move(ext.highest);
                    continue;
                }

                // Whenever there is current min and max, then just update if necessary
                if (ext.min_v < batch_min) {
                    batch_min = ext.min_v;
                    batch_lowest = move(ext.lowest);
                } else if (ext.min_v == batch_min) {
                    batch_lowest.merge(ext.lowest);
                }
                if (ext.max_v > batch_max) {
                    batch_max = ext.max_v;
                    batch_highest = move(ext.highest);
                } else if (ext.max_v == batch_max) {
                    batch_highest.merge(ext.highest);
                }
            }

            // Update min-max data between batches
            if (batch_valid) {
                if (!valid) {
                    valid = true;
                    min_val = batch_min;
                    max_val = batch_max;
                    lowest_functions = move(batch_lowest);
                    highest_functions = move(batch_highest);
                } else {
                    if (batch_min < min_val) {
                        min_val = batch_min;
                        lowest_functions = move(batch_lowest);
                    } else if (batch_min == min_val) {
                        lowest_functions.merge(batch_lowest);
                    }

                    if (batch_max > max_val) {
                        max_val = batch_max;
                        highest_functions = move(batch_highest);
                    } else if (batch_max == max_val) {
                        highest_functions.merge(batch_highest);
                    }
                }
            }
            ++nbatch;

            auto now = chrono::steady_clock::now();
            chrono::duration<double> elapsed = now - start_time;
            double time_per_batch = elapsed.count() / nbatch;
            double remaining_time = (NUM_BATCHES - nbatch) * time_per_batch;

            if (!first_batch)
                CLEAR_LINES(4)
            cout << "Done batch no: " << nbatch << ", ETA: " << format_time(remaining_time) << endl;
            if (valid) {
                cout << "Min function (" << *lowest_functions.begin() << ") of a total of (" << lowest_functions.size() << ") with value (" << min_val << ")\n"
                     << "Max function (" << *highest_functions.begin() << ") of a total of (" << highest_functions.size() << ") with value (" << max_val << ")" << endl;
            } else {
                cout << "No sampled functions yet in this run.\n"
                     << "Current PROB value skipped every function so far." << endl;
            }
            
            first_batch = false;
        }
        file.close();

        // Print results to user
        if (!valid) {
            CLEAR_LINES(3)
            cout << "No sampled functions found. Increase --prob and try again." << endl;
            return -1;
        }

        auto now = chrono::steady_clock::now();
        chrono::duration<double> elapsed = now - start_time;
        CLEAR_LINES(3)
        cout    << "Computed max and min values! Time spent: " << format_time(elapsed.count()) << "\n"
                << "--------------------------------------------------------------------------------" << endl
                << "Number of functions with highest value (" << max_val << ") of metric: " << highest_functions.size() << "\n"
                << "Sample of up to 10:\n";
        {
            int shown = 0;
            for (uint32_t x : highest_functions) {
                if (shown++ >= 10) break;
                cout << "----> " << x << "\n";
            }
            cout << endl;
        }
        cout << "Number of functions with lowest value (" << min_val << ") of metric: " << lowest_functions.size() << "\n";
        cout << "Sample of up to 10:\n";
        {
            int shown = 0;
            for (uint32_t x : lowest_functions) {
                if (shown++ >= 10) break;
                cout << "----> " << x << "\n";
            }
            cout << endl;
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

        cout << "Highest functions saved in: " << high_path << endl;
        cout << "Lowest  functions saved in: " << low_path  << endl;
        cout << "--------------------------------------------------------------------------------" << endl;
    }
    // ─────────────────────────────────────────────────────────────────────────

    // ── Step 2: calculate bins ───────────────────────────────────────────────
    // Build bin edges:  NUM_BINS+1 edges from min_val to max_val  (inclusive)
    ull range = max_val - min_val + 1;
    ull NUM_BINS = min(MAX_NUM_BINS, range);
    ull bin_size = (range + NUM_BINS - 1) / NUM_BINS;
    vector<ull> hist_accum(NUM_BINS, 0);

    cout << "Computing histogram..." << endl;
    ifstream file(functions_filepath, ios::binary);
    ull nbatch = 0;

    auto start_time = chrono::steady_clock::now();
    // Read binary file in batches
    while (file.read(reinterpret_cast<char*>(buffer.data()), buffer.size() * sizeof(uint32_t)) || file.gcount() > 0) {
        ull num_elements = static_cast<ull>(file.gcount()) / sizeof(uint32_t);

        if (num_elements <= 0) {
            cerr << "Some bytes readed from " << functions_filepath << ", but not a whole element. Probably the file is corrupted" << endl;
            return -1;
        }
        
        // Divide the processing within a batch among num_threads to speed up the computation
        unsigned num_threads = static_cast<unsigned>(min<ull>(hw_threads, num_elements));
        ull thread_chunk = (num_elements + num_threads - 1) / num_threads;
        vector<vector<ull>> local_histograms(num_threads, vector<ull>(NUM_BINS, 0));
        vector<thread> threads;
        threads.reserve(num_threads);

        for (unsigned thread_id = 0; thread_id < num_threads; ++thread_id) {
            // Compute its range
            ull start = thread_id * thread_chunk;
            ull end = min(num_elements, start + thread_chunk);

            // Build the procedure to process its chunk
            threads.emplace_back([&, thread_id, start, end]() {
                ThreadRNG& random_gen = rng_pool[thread_id];
                vector<ull>& local_hist = local_histograms[thread_id];
                for (ull i = start; i < end; ++i) {
                    if (random_gen.next() > PROB)
                        continue;

                    truthtable t = to_truthtable(buffer[i]);
                    ull v = METRIC(t);
                    ull bin = (v - min_val) / bin_size;
                    if (bin >= NUM_BINS) bin = NUM_BINS - 1;
                    local_hist[bin]++;
                }
            });
        }

        // Start each thread and wait until all have finished
        for (thread& worker : threads)
            worker.join();

        // Rebuild global computation based on local computations
        for (const auto& local_hist : local_histograms) {
            for (ull b = 0; b < NUM_BINS; ++b) {
                hist_accum[b] += local_hist[b];
            }
        }
        ++nbatch;

        auto now = chrono::steady_clock::now();
        chrono::duration<double> elapsed = now - start_time;
        double time_per_batch = elapsed.count() / nbatch;
        double remaining_time = (NUM_BATCHES - nbatch) * time_per_batch;

        CLEAR_LINES(2)
        cout    << "Done batch no: " << nbatch
                << ", ETA: " << format_time(remaining_time) << endl;

    }
    auto now = chrono::steady_clock::now();
    chrono::duration<double> elapsed = now - start_time;
    cout << CLEAR_LINE << "Computed histogram! Time spent: " << format_time(elapsed.count()) << endl;
    file.close();
    // ─────────────────────────────────────────────────────────────────────────

    // ── Step 3: Save histogram as CSV ────────────────────────────────────────
    {
        fs::create_directories(metric_results_path, ec);
        ofstream fh(hist_path);
        if (!fh) {
            cerr << "Cannot write " << hist_path << endl;
            return -1;
        }

        fh << "bin_left,bin_right,count\n";
        for (int b = 0; b < NUM_BINS; ++b) {
            int bin_start = min_val + b * bin_size;
            int bin_end   = bin_start + bin_size - 1;
            fh << bin_start << "," << bin_end << "," << hist_accum[b] << "\n";
        }
        cout << "Histogram saved -> " << hist_path << endl;
    }
    // ─────────────────────────────────────────────────────────────────────────

    return 0;
}

int parse_argument(int argc, char * argv[]) {
    
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if ((arg == "-b" || arg == "--batch") && i + 1 < argc) {
            BATCH_SIZE = stoull(argv[++i]);
        } else if ((arg == "-p" || arg == "--prob") && i + 1 < argc) {
            PROB = stof(argv[++i]);
        } else if ((arg == "-s" || arg == "--seed") && i + 1 < argc) {
            RNG_BASE_SEED = stoull(argv[++i]);
            RNG_SEED_FROM_CLI = true;
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
            print_usage(argv[0]);
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

void print_usage(const char* prog) {
    cerr << "Usage: " << prog << " -m METRIC -c CLASS [OPTIONS]\n\n"
         << "Required:\n"
         << "  -m, --metric  METRIC    Metric to use. Available:";
    for (auto& [name, _] : METRICS) cerr << " " << name;
    cerr << "\n"
         << "  -c, --class   CLASS     Complexity class to analyse\n\n"
         << "Options:\n"
         << "  -b, --batch   SIZE      Batch size (default: " << BATCH_SIZE << ")\n"
         << "  -p, --prob    PROB      Probability threshold (default: " << PROB << ")\n"
         << "  -s, --seed    SEED      RNG base seed (default: random_device)\n\n"
         << "Examples:\n"
         << "  " << prog << " --metric entropy --class NP\n"
         << "  " << prog << " --metric entropy --class NP --batch 512 --prob 0.75\n"
         << "  " << prog << " --metric entropy --class NP --seed 123456789\n";
}
