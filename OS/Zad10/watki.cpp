#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <string>
using namespace std;

void loop(int n) {
    cout << "Watek numer " << n << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2 || (argc == 3 && stoi(argv[2]) < 1)) {
        cerr << "Usage: <path_to_text_file> (Optional: <number_of_threads> >= 1)" << endl;
        return 1;
    }
    int num_threads = thread::hardware_concurrency();
    if (argc == 3) {
        num_threads = stoi(argv[2]);
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << argv[1] << endl;
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();
    file.close();

    cout << "Loaded " << content.size() << " bytes from " << argv[1] << endl;

    thread t1(loop, 1);
    thread t2(loop, 2);
    t1.join();
    t2.join();
    return 0;
}