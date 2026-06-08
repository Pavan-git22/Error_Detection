#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <random>
#include <algorithm>

using namespace std;
using namespace chrono;

mutex mtx;

struct Result {
    int id;
    double latency;
    bool success;
};

vector<Result> results;

void simulate_request(int id) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> delay(50, 300);
    uniform_int_distribution<> fail(1, 10);

    auto start = high_resolution_clock::now();

    this_thread::sleep_for(milliseconds(delay(gen)));

    auto end = high_resolution_clock::now();
    double latency = duration<double, milli>(end - start).count();

    bool success = fail(gen) > 2;

    // Safer locking
    lock_guard<mutex> lock(mtx);
    results.push_back({id, latency, success});
}

int main() {
    int total_requests = 20;
    vector<thread> threads;

    // Create threads
    for (int i = 1; i <= total_requests; i++) {
        threads.emplace_back(simulate_request, i);
    }

    // Join threads
    for (auto &t : threads) {
        t.join();
    }

    // Optional: sort results by ID
    sort(results.begin(), results.end(), [](Result a, Result b) {
        return a.id < b.id;
    });

    cout << "\n===== RESULT TABLE =====\n";
    cout << "ID\tLatency(ms)\tStatus\n";

    int success_count = 0;

    for (auto &r : results) {
        cout << r.id << "\t" << r.latency << "\t\t"
             << (r.success ? "Success" : "Fail") << endl;

        if (r.success) success_count++;
    }

    cout << "\nSuccess Rate: "
         << (success_count * 100.0 / total_requests) << "%\n";

    return 0;
} 