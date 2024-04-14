#include <thread>
#include <string>
#include <vector>
#include <iostream>
#include "simulator.h"
#include "inputParsers.h"
#include "metrics.h"

void runSimulatorInstance(int instanceNumber, int n_iter, int n_threads, int n_pages, int n_size_of_page_pool, int n_CN){
    std::cout << "Started execution by thread no.:\t" << instanceNumber << std::endl;
    BruteForceOptimalScheduler bfs;
    GreedySchedulingAlgorithm gs(metric2);
    std::string filename {"output"}; filename += std::to_string(instanceNumber); filename += ".txt";
    for(int i = 0; i < n_iter; ++i){
        RandomInputParser rip(n_threads, n_pages, n_size_of_page_pool);
        const Input* r_input_ptr = rip.getInputPtr();
        auto brute_placement = bfs.runAlgorithm(r_input_ptr, n_CN, metric2);
        auto greedy_placement = gs.runAlgorithm(r_input_ptr, n_CN);
        brute_placement.writeOutputToFile(metric2(greedy_placement), metric2(brute_placement), filename);
    }
}

int main(){
    std::vector<std::thread> threads;

}