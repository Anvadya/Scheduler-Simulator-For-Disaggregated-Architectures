#include <thread>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include "inputParsers.h"
#include "simulator.h"
#include "metrics.h"

//The thread for running an instance
void runSimulatorInstance(int instanceNumber, int n_iter, int n_threads, int n_pages, int n_size_of_page_pool, int n_CN){
    // std::cout << "Started execution by thread no.:\t" << instanceNumber << std::endl;
    BruteForceOptimalScheduler bfs;
    GreedySchedulingAlgorithm gs(metric2);
    std::string filename {"./outputs/output"}; filename += std::to_string(instanceNumber); filename += ".txt";
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < n_iter; ++i){
        RandomInputParser rip(n_threads, n_pages, n_size_of_page_pool);
        const Input* r_input_ptr = rip.getInputPtr();
        auto brute_placement = bfs.runAlgorithm(r_input_ptr, n_CN, metric2);
        auto greedy_placement = gs.runAlgorithm(r_input_ptr, n_CN);
        brute_placement.writeOutputToFile(metric2(greedy_placement), metric2(brute_placement), filename);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Elapsed time by thread:\t" << instanceNumber << "is :\t" << elapsed.count() << " ms" << std::endl;
}

const int no_of_threads = 8;
const int n_tests = 96/no_of_threads;

int main(){
    std::vector<std::thread> threads;
    for(int j = 0; j < n_tests; ++j){
        for(int i = 0; i < no_of_threads; ++i) threads.emplace_back(runSimulatorInstance, no_of_threads*j+i, 10, 4+2*j, 10, 25, 4);
    }
    for(auto& t: threads) t.join();
}
