// Procesowa wersja: mmap pliku, anonimowe mmap dla współdzielonej struktury
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstring>
#include <cerrno>
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

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_ascii_file> [num_processes]" << std::endl;
        return 1;
    }

    int num_processes = 0;
    if (argc > 2) {
        num_processes = std::stoi(argv[2]);
        if (num_processes <= 0) num_processes = 1;
    } else {
        long procs = sysconf(_SC_NPROCESSORS_ONLN);
        num_processes = (procs > 0) ? static_cast<int>(procs) : 1;
    }

    const char* path = argv[1];
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        std::cerr << "Error opening file '" << path << "': " << strerror(errno) << std::endl;
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        std::cerr << "fstat failed: " << strerror(errno) << std::endl;
        close(fd);
        return 1;
    }

    off_t file_size = st.st_size;
    if (file_size == 0) {
        std::cout << "File is empty." << std::endl;
        close(fd);
        return 0;
    }

    void* file_map = mmap(nullptr, static_cast<size_t>(file_size), PROT_READ, MAP_SHARED, fd, 0);
    if (file_map == MAP_FAILED) {
        std::cerr << "mmap file failed: " << strerror(errno) << std::endl;
        close(fd);
        return 1;
    }

    // Przydzielenie anonimowej współdzielonej pamięci dla SharedData
    void* shm = mmap(nullptr, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shm == MAP_FAILED) {
        std::cerr << "mmap shared memory failed: " << strerror(errno) << std::endl;
        munmap(file_map, static_cast<size_t>(file_size));
        close(fd);
        return 1;
    }

    SharedData* shared = static_cast<SharedData*>(shm);
    // Inicjalizacja danych współdzielonych
    memset(shared->counts, 0, sizeof(shared->counts));
    shared->sum_sqrt_ascii = 0.0;

    // Inicjalizacja mutexu współdzielonego między procesami
    pthread_mutexattr_t mattr;
    if (pthread_mutexattr_init(&mattr) != 0) {
        std::cerr << "pthread_mutexattr_init failed" << std::endl;
        munmap(shm, sizeof(SharedData));
        munmap(file_map, static_cast<size_t>(file_size));
        close(fd);
        return 1;
    }
    if (pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED) != 0) {
        std::cerr << "pthread_mutexattr_setpshared failed" << std::endl;
        pthread_mutexattr_destroy(&mattr);
        munmap(shm, sizeof(SharedData));
        munmap(file_map, static_cast<size_t>(file_size));
        close(fd);
        return 1;
    }
    if (pthread_mutex_init(&shared->mutex, &mattr) != 0) {
        std::cerr << "pthread_mutex_init failed" << std::endl;
        pthread_mutexattr_destroy(&mattr);
        munmap(shm, sizeof(SharedData));
        munmap(file_map, static_cast<size_t>(file_size));
        close(fd);
        return 1;
    }
    pthread_mutexattr_destroy(&mattr);

    if (static_cast<off_t>(num_processes) > file_size) {
        num_processes = static_cast<int>(file_size);
        if (num_processes == 0) num_processes = 1;
    }

    size_t chunk = static_cast<size_t>(file_size) / static_cast<size_t>(num_processes);
    size_t remainder = static_cast<size_t>(file_size) % static_cast<size_t>(num_processes);

    std::vector<pid_t> children;
    children.reserve(num_processes);

    unsigned char* data = reinterpret_cast<unsigned char*>(file_map);

    for (int i = 0; i < num_processes; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            std::cerr << "fork failed: " << strerror(errno) << std::endl;
            break;
        }
        if (pid == 0) {
            // proces podrzędny
            size_t start = static_cast<size_t>(i) * chunk + static_cast<size_t>(std::min(i, static_cast<int>(remainder)));
            // Rozdziel resztę na pierwsze 'remainder' kawałków
            if (static_cast<size_t>(i) < remainder) start = static_cast<size_t>(i) * (chunk + 1);
            size_t end = start + chunk + (static_cast<size_t>(i) < remainder ? 1 : 0);
            if (end > static_cast<size_t>(file_size)) end = static_cast<size_t>(file_size);

            unsigned int localCount[LETTER_COUNT];
            memset(localCount, 0, sizeof(localCount));
            double localSum = 0.0;

            for (size_t j = start; j < end; ++j) {
                unsigned char c = data[j];
                localSum += sqrt(static_cast<double>(c));
                if (c >= 'A' && c <= 'Z') {
                    localCount[c - 'A']++;
                } else if (c >= 'a' && c <= 'z') {
                    localCount[c - 'a']++;
                }
            }

            // Wyniki lokalne do pamięci współdzielonej pod mutexem
            if (pthread_mutex_lock(&shared->mutex) != 0) {
                _exit(2);
            }
            for (int k = 0; k < LETTER_COUNT; ++k) shared->counts[k] += localCount[k];
            shared->sum_sqrt_ascii += localSum;
            pthread_mutex_unlock(&shared->mutex);

            munmap(file_map, static_cast<size_t>(file_size));
            munmap(shm, sizeof(SharedData));
            close(fd);
            _exit(0);
        } else {
            children.push_back(pid);
        }
    }

    // Czekanie na zakończenie procesów podrzędnych
    int status = 0;
    for (pid_t cpid : children) {
        waitpid(cpid, &status, 0);
    }

    for (int i = 0; i < LETTER_COUNT; ++i) {
        std::cout << static_cast<char>('a' + i) << ": " << shared->counts[i] << std::endl;
    }
    std::cout << "Sum sqrt ASCII: " << shared->sum_sqrt_ascii << std::endl;

    // Czyszczenie zasobów
    pthread_mutex_destroy(&shared->mutex);
    munmap(shm, sizeof(SharedData));
    munmap(file_map, static_cast<size_t>(file_size));
    close(fd);

    return 0;
}
