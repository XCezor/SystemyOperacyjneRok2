#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <cmath>
#include <algorithm>

using namespace std;

#define LETTER_COUNT 26

struct context {
    string content;
    unsigned long count[LETTER_COUNT] = {0};
    double sumOfSquares = 0;
    mutex mutex;
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

    lock_guard<std::mutex> lock(ctx.mutex);
    ctx.sumOfSquares += localSum;
    for (int i = 0; i < LETTER_COUNT; ++i) {
        ctx.count[i] += localCount[i];
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <file> [threads]" << endl;
        return 1;
    }

    // Ustalanie liczby wątków
    unsigned int numThreads = thread::hardware_concurrency();
    if (argc >= 3) {
        int t = stoi(argv[2]);
        if (t > 0) numThreads = t;
    }
    if (numThreads == 0) numThreads = 1;

    ifstream file(argv[1], ios::binary | ios::ate);
    if (!file) {
        cerr << "Error: Cannot open file " << argv[1] << endl;
        return 1;
    }

    context ctx;
    auto fileSize = file.tellg();
    ctx.content.resize(fileSize);
    file.seekg(0);
    file.read(&ctx.content[0], fileSize);
    file.close();

    if (fileSize == 0) {
        cout << "File is empty." << endl;
        return 0;
    }

    if (fileSize < numThreads) numThreads = fileSize;

    vector<thread> threads;
    size_t chunkSize = fileSize / numThreads;
    size_t remainder = fileSize % numThreads;
    size_t start = 0;

    cout << "Processing " << fileSize << " bytes with " << numThreads << " threads..." << endl;

    for (unsigned int i = 0; i < numThreads; ++i) {
        // Ostatni wątek bierze resztę z dzielenia
        size_t end = start + chunkSize + (i < remainder ? 1 : 0);
        threads.emplace_back(worker, ref(ctx), start, end);
        start = end;
    }

    // Czekanie na zakończenie wszystkich wątków
    for (auto& t : threads) {
        t.join();
    }

    cout << "Sum sqrt ASCII: " << ctx.sumOfSquares << endl;
    for (int i = 0; i < LETTER_COUNT; ++i) {
        cout << static_cast<char>('a' + i) << ": " << ctx.count[i] << endl;
    }

    return 0;
}