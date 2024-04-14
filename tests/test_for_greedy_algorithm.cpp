#include <iostream>
#include <vector>
#include <functional>
#include "../inputParsers.h"
#include "../input.h"
#include "../thread.h"
#include "../simulator.h"
#include "../metrics.h"

int main(){
    RandomInputParser r_parser(2, 3, 5);
    // TextInputParser t_parser("test_input.txt");
    const Input* t_input_ptr = r_parser.getInputPtr();
    // for(const auto c: t_input_ptr->threads){
    //     for(auto d: (c->pages).pages){
    //         std::cout << d << " ";
    //     }
    //     std::cout << std::endl;
    // }
    GreedySchedulingAlgorithm gs{metric2};
    // std::cout << "Initializing the scheduler." << std::endl;

    auto placemenet_strategy = gs.runAlgorithm(t_input_ptr, 1);
    // placemenet_strategy.displayPlacementOfThreads();
    placemenet_strategy.writeOutputToFile(0,0, "test_output.txt");
    return 0;
}