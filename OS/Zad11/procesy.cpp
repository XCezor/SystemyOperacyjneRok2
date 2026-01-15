// Concise process-based implementation: mmap file, anonymous shared mmap for SharedData, fork, merge under pthread mutex
#include <iostream>
#include <cmath>
#include <cstring>
#include <vector>
#include <cstdlib>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

#define LETTER_COUNT 26

struct SharedData {
    unsigned int counts[LETTER_COUNT];
    double sum_sqrt_ascii;
    pthread_mutex_t mutex;
};

int main(int argc, char** argv) {
    if (argc < 2) return 1;
    int n = (argc > 2) ? std::atoi(argv[2]) : (int)sysconf(_SC_NPROCESSORS_ONLN);
    if (n <= 0) n = 1;

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) return 1;
    struct stat st; if (fstat(fd, &st) == -1) { close(fd); return 1; }
    size_t sz = (size_t)st.st_size;
    if (sz == 0) { close(fd); return 0; }

    unsigned char* data = (unsigned char*)mmap(nullptr, sz, PROT_READ, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) { close(fd); return 1; }

    SharedData* shared = (SharedData*)mmap(nullptr, sizeof(SharedData), PROT_READ|PROT_WRITE,
                                           MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if (shared == MAP_FAILED) { munmap(data, sz); close(fd); return 1; }

    memset(shared->counts, 0, sizeof(shared->counts));
    shared->sum_sqrt_ascii = 0.0;

    pthread_mutexattr_t mattr; pthread_mutexattr_init(&mattr);
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shared->mutex, &mattr);
    pthread_mutexattr_destroy(&mattr);

    if ((size_t)n > sz) n = (int)sz;
    size_t base = sz / n, rem = sz % n;

    std::vector<pid_t> kids; kids.reserve(n);
    for (int i = 0; i < n; ++i) {
        pid_t pid = fork();
        if (pid < 0) break;
        if (pid == 0) {
            size_t start = (size_t)i * base + ((size_t)i < rem ? (size_t)i : rem);
            if ((size_t)i < rem) start = (size_t)i * (base + 1);
            size_t len = base + ((size_t)i < rem ? 1 : 0);
            size_t end = start + len; if (end > sz) end = sz;

            unsigned int local[LETTER_COUNT]; memset(local, 0, sizeof(local));
            double lsum = 0.0;
            for (size_t j = start; j < end; ++j) {
                unsigned char c = data[j];
                lsum += sqrt((double)c);
                if (c >= 'A' && c <= 'Z') local[c - 'A']++;
                else if (c >= 'a' && c <= 'z') local[c - 'a']++;
            }

            pthread_mutex_lock(&shared->mutex);
            for (int k = 0; k < LETTER_COUNT; ++k) shared->counts[k] += local[k];
            shared->sum_sqrt_ascii += lsum;
            pthread_mutex_unlock(&shared->mutex);

            munmap(data, sz); munmap(shared, sizeof(SharedData)); close(fd);
            _exit(0);
        }
        kids.push_back(pid);
    }

    for (pid_t p : kids) waitpid(p, nullptr, 0);

    for (int i = 0; i < LETTER_COUNT; ++i) std::cout << (char)('a' + i) << ": " << shared->counts[i] << "\n";
    std::cout << "Sum sqrt ASCII: " << shared->sum_sqrt_ascii << '\n';

    pthread_mutex_destroy(&shared->mutex);
    munmap(shared, sizeof(SharedData)); munmap(data, sz); close(fd);
    return 0;
}