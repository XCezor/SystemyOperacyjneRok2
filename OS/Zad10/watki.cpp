#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <cmath>
#include <algorithm>

#define LETTER_COUNT 26

struct context {
    std::string content;
    unsigned long count[LETTER_COUNT] = {0};
    double sumOfSquares = 0;
    std::mutex mutex;
};

void worker(context& ctx, size_t start, size_t end) {
    unsigned long localCount[LETTER_COUNT] = {0};
    double localSum = 0.0;
    const char* data = ctx.content.data(); // Bezpośredni dostęp do wskaźnika

    for (size_t i = start; i < end; ++i) {
        unsigned char c = static_cast<unsigned char>(data[i]);

        localSum += sqrt(static_cast<double>(c));

        if (c >= 'A' && c <= 'Z') c += 32;
        if (c >= 'a' && c <= 'z') {
            localCount[c - 'a']++;
        }
    }

    std::lock_guard<std::mutex> lock(ctx.mutex);
    ctx.sumOfSquares += localSum;
    for (int i = 0; i < LETTER_COUNT; ++i) {
        ctx.count[i] += localCount[i];
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file> [threads]" << std::endl;
        return 1;
    }

    // Ustalanie liczby wątków
    unsigned int numThreads = std::thread::hardware_concurrency();
    if (argc >= 3) {
        int t = std::stoi(argv[2]);
        if (t > 0) numThreads = static_cast<unsigned int>(t);
    }
    if (numThreads == 0) numThreads = 1;

    std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Error: Cannot open file " << argv[1] << std::endl;
        return 1;
    }

    context ctx;
    auto fileSize = file.tellg();
    ctx.content.resize(static_cast<std::string::size_type>(fileSize));
    file.seekg(0);
    file.read(&ctx.content[0], fileSize);
    file.close();

    if (fileSize == 0) {
        std::cout << "File is empty." << std::endl;
        return 0;
    }

    if (fileSize < static_cast<std::streamoff>(numThreads)) numThreads = static_cast<unsigned int>(fileSize);

    std::vector<std::thread> threads;
    size_t chunkSize = static_cast<size_t>(fileSize) / numThreads;
    size_t remainder = static_cast<size_t>(fileSize) % numThreads;
    size_t start = 0;

    std::cout << "Processing " << fileSize << " bytes with " << numThreads << " threads..." << std::endl;

    for (unsigned int i = 0; i < numThreads; ++i) {
        // Ostatni wątek bierze resztę z dzielenia
        size_t end = start + chunkSize + (i < remainder ? 1 : 0);
        threads.emplace_back(worker, std::ref(ctx), start, end);
        start = end;
    }

    // Czekanie na zakończenie wszystkich wątków
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Sum sqrt ASCII: " << ctx.sumOfSquares << std::endl;
    for (int i = 0; i < LETTER_COUNT; ++i) {
        std::cout << static_cast<char>('a' + i) << ": " << ctx.count[i] << std::endl;
    }

    return 0;
}