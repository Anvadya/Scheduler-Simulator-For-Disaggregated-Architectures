#include <iostream>
#include <vector>
#include <functional>
#include "../inputParsers.h"
#include "../input.h"
#include "../thread.h"
#include "../simulator.h"
#include "../metrics.h"

int main(){
    RandomInputParser r_parser(5, 3, 5);
    // TextInputParser t_parser("test_input.txt");
    const Input* t_input_ptr = r_parser.getInputPtr();
    for(const auto c: t_input_ptr->threads){
        for(auto d: (c->pages).pages){
            std::cout << d << " ";
        }
        std::cout << std::endl;
    }
    BruteForceOptimalScheduler bf_scheduler;

    // std::function<double(const PlacementStrategy&)> f_ptr = &metric1;
    // auto placemenet_strategy = bf_scheduler.runAlgorithm(t_input_ptr, 2, &metric1);
    // std::cout << "NO ERROR ENCOUNTERED YET" << std::endl;
    auto placemenet_strategy = bf_scheduler.runAlgorithm(t_input_ptr, 2, &metric2);
    placemenet_strategy.displayPlacementOfThreads();
    placemenet_strategy.writeOutputToFile(0,0, "test_output.txt");
    return 0;
}