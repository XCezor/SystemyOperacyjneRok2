#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <vector>
#include <mutex>
#include <cmath>

struct SharedData {
    unsigned int counts[26];
    double sum_sqrt_ascii;
    std::mutex mutex;
};

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: <path_to_ascii_file>" << " Optional: <number_of_processes>" << std::endl;
        exit(1);
    }
    int num_processes = (argc > 2) ? std::stoi(argv[2]) : std::thread::hardware_concurrency();

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }
    std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Alokacja pamięci współdzielonej
    SharedData *shared_data = new SharedData();
    memset(shared_data->counts, 0, sizeof(shared_data->counts));
    shared_data->sum_sqrt_ascii = 0.0;

    // Przetwarzanie zawartości pliku w wielu wątkach
    std::vector<std::thread> threads;
    size_t chunk_size = file_content.size() / num_processes;
    for (int i = 0; i < num_processes; ++i) {
        threads.emplace_back([=]() {
            size_t start = i * chunk_size;
            size_t end = (i == num_processes - 1) ? file_content.size() : start + chunk_size;
            for (size_t j = start; j < end; ++j) {
                char c = file_content[j];
                if (c >= 'A' && c <= 'Z') {
                    std::lock_guard<std::mutex> lock(shared_data->mutex);
                    shared_data->counts[c - 'A']++;
                } else if (c >= 'a' && c <= 'z') {
                    std::lock_guard<std::mutex> lock(shared_data->mutex);
                    shared_data->counts[c - 'a']++;
                }
                shared_data->sum_sqrt_ascii += sqrt(static_cast<double>(c));
            }
        });
    }

    // Czekanie na wątki
    for (auto &t : threads) {
        t.join();
    }

    for (int i = 0; i < 26; ++i) {
        std::cout << "Count of " << char('A' + i) << ": " << shared_data->counts[i] << std::endl;
    }
    std::cout << "Sum of square roots of ASCII values: " << shared_data->sum_sqrt_ascii << std::endl;

    delete shared_data;
    return 0;
}
