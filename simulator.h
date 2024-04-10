#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>
#include <string>
#include <iostream>
#include <limits>
#include <functional>
#include "input.h"
#include "metrics.h"

struct PlacementStrategy{
private:
public:
    Input* threads_to_be_placed = nullptr;
    int n_CN;
    std::vector<int> assignment_of_CN;

    PlacementStrategy(Input* thread_set, int CN):threads_to_be_placed(thread_set), n_CN(CN), assignment_of_CN(CN, 0){}
    PlacementStrategy(const PlacementStrategy& inputPlacement) = default;
    PlacementStrategy& operator=(const PlacementStrategy& otherPlacement) = default;

    bool isDefaultPlacement(){
        for(auto c: assignment_of_CN) if(c) return false;
        return true;
    }
    void writeOutputToFile(const std::string& filename = "output.txt", double metric_value) const{
        std::ofstream file(filename, std::ios::app); // Open file in append mode
        file << n_CN << ", " << metric_value << std::endl;
        file.close();
    }
    bool isValidPlacement() const{
        int curMax = 0;
        for(auto c: assignment_of_CN){
            if(c > (curMax+1)) return false;
            curMax = std::max(c, curMax);
        }
        return true;
    }
    PlacementStrategy& operator++(){
        do{
            int carry = 1;
            for(int j = int(assignment_of_CN.size()) - 1; j >= 0; --j){
                if((assignment_of_CN[j] += carry) < n_CN) break;
                assignment_of_CN[j] = 0;
            }
        } while(!isValidPlacement());
        return *this;
    }
};

class SchedulingAlgorithm{
private:
public:
    SchedulingAlgorithm() = default;
    static const PlacementStrategy& runAlgorithm(Input* input, int n_CN);

    virtual ~SchedulingAlgorithm() = default;
};

class BruteForceOptimalScheduler : public SchedulingAlgorithm{
private:
public:
    BruteForceOptimalScheduler() = default;
    static const PlacementStrategy& runAlgorithm(Input* input, int n_CN, std::function<double(const PlacementStrategy*)> metric){
        std::cerr << "Brute Force" << std::endl;

        PlacementStrategy* placement = new PlacementStrategy(input, n_CN);
        PlacementStrategy bestPlacement(*placement);
        double curMax = std::numeric_limits<int>::min();
        do{
            double current_placement_metric = metric(placement);
            if(current_placement_metric > curMax){
                curMax = current_placement_metric;
                bestPlacement = *placement;
            }
        }while(!((++placement)->isDefaultPlacement()));
        *placement = bestPlacement;
        return *placement;
    }

    virtual ~BruteForceOptimalScheduler() = default;
};

#endif