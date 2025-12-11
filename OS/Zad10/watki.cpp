#include <iostream>
#include <thread>
using namespace std;

void loop(int n) {
    cout << "Wątek numer " << n << endl;
}

int main() {
    thread t1(loop, 1);
    thread t2(loop, 2);
    t1.join();
    t2.join();
    return 0;
}