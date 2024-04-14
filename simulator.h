#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>
#include <string>
#include <iostream>
#include <limits>
#include <functional>
#include <numeric>
#include <utility>
#include <limits>
#include "input.h"

struct PlacementStrategy{
private:
public:
    const Input* threads_to_be_placed = nullptr;
    int n_CN;
    std::vector<int> assignment_of_CN;

    PlacementStrategy(const Input* thread_set, int CN):threads_to_be_placed(thread_set), n_CN(CN), assignment_of_CN(thread_set->numberOfThreads(), 0){}
    PlacementStrategy(const PlacementStrategy& inputPlacement) = default;
    PlacementStrategy& operator=(const PlacementStrategy& otherPlacement) = default;

    void displayPlacementOfThreads() const{
        for(int i = 0; i < threads_to_be_placed->numberOfThreads(); ++i){
            std::cout << "Thread " << i << " is placed on Compute Node #" << assignment_of_CN[i] << std::endl;
        }
    }
    bool isDefaultPlacement() const{
        for(auto c: assignment_of_CN) if(c) return false;
        return true;
    }
    //The format is: #CN, greedy metric, brute force metric, (#threads, thread placement)-> this is the same as input from text file format
    void writeOutputToFile(double greedy_metric_value, double optimal_metric_value, const std::string& filename = "output.txt") const{
        std::ofstream file(filename, std::ios::app); // Open file in append mode
        file << n_CN << ", ";
        file << greedy_metric_value << ", " << optimal_metric_value << ", ";
        file << threads_to_be_placed->numberOfThreads() << ", ";
        for(const auto& thread: threads_to_be_placed->threads){
            file << (thread->pages).size() << ", ";
            for(const auto& page_in_thread: (thread->pages).pages){
                file << page_in_thread << ", ";
            }
        }
        file << "\n";
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
        // std::cerr << "Entered inside the ++ operator of PlacementStrategy" << std::endl;
        do{
            int carry = 1;
            for(int j = int(assignment_of_CN.size()) - 1; j >= 0; --j){
                if((assignment_of_CN[j] += carry) < n_CN) break;
                assignment_of_CN[j] = 0;
            }
        // for(auto c: assignment_of_CN) std::cout << c << " "; std::cout << std::endl;
        } while(!isValidPlacement());
        return *this;
    }
    const std::vector<std::vector<Thread*>> getCN_wise_thread_allocation() const{
        if(!threads_to_be_placed) return std::vector<std::vector<Thread*>>{};
        std::vector<std::vector<Thread*>> placement_CN_wise(n_CN);
        for(int i = 0; i < threads_to_be_placed->numberOfThreads(); ++i){
            placement_CN_wise[assignment_of_CN[i]].push_back(threads_to_be_placed->threads[i]);
        }
        return placement_CN_wise;
    }
};

// #include "metrics.h"

class SchedulingAlgorithm{
private:
public:
    SchedulingAlgorithm() = default;
    // static const PlacementStrategy& runAlgorithm(const Input* input, int n_CN);

    virtual ~SchedulingAlgorithm() = default;
};

class BruteForceOptimalScheduler : public SchedulingAlgorithm{
private:
public:
    BruteForceOptimalScheduler() = default;
    static const PlacementStrategy& runAlgorithm(const Input* input, int n_CN, std::function<double(const PlacementStrategy&)> metric){
        // std::cerr << "Inside Brute Force's runAlgorithm method" << std::endl;

        //<------------------------IMPORTANT--------------------------->
        //TODO: FIX POTENTIAL MEMORY LEAKS
        PlacementStrategy* placement = new PlacementStrategy(input, n_CN);
        PlacementStrategy bestPlacement(*placement);
        double curMax = std::numeric_limits<int>::min();
        do{
            double current_placement_metric = metric(*placement);
            // std::cerr << "Metric value given by metric function inside the BF Scheduler:\t" << (current_placement_metric) << std::endl;
            if(current_placement_metric > curMax){
                curMax = current_placement_metric;
                bestPlacement = *placement;
            }
        }while(!((++(*placement)).isDefaultPlacement()));

        // std::cerr << "Finished the main loop inside BF Scheduler's runAlgo method" << std::endl;

        *placement = bestPlacement;
        return *placement;
    }

    virtual ~BruteForceOptimalScheduler() = default;
};


class GreedySchedulingAlgorithm: public SchedulingAlgorithm{
private:
    const std::function<double(const PlacementStrategy&)> metric;
    mutable const Input* input = nullptr;

    int sizeOfUnionOfAllThreadsInSet(const std::set<int>& s1) const{
        // std::cerr << "Checkpoint 1.1" << std::endl;
        std::set<int> temp_set;
        for(auto c: s1){
            auto& set_of_pages = *(input->pages_in_PageSet(c));
            temp_set.insert(set_of_pages.begin(), set_of_pages.end());
        }
        return temp_set.size();
    }

    int sizeOfIntersectionOfTwoPageSets(int s1, int s2) const{
        // std::cerr << "Checkpoint f2.1" << std::endl;
        const std::set<int>& ps1 = *(input->pages_in_PageSet(s1));
        const std::set<int>& ps2 = *(input->pages_in_PageSet(s2));
        int metric = 0;
        for(auto c: ps1) if(ps2.find(c) != ps2.end()) ++metric;
        // std::cerr << "Intersection:\t" << metric;
        return metric;
    }

    double calculateMetricForSetCombination(const std::pair<std::set<int>,double>& p1, const std::pair<std::set<int>,double>& p2) const{
        // std::cerr << "Checkpoint f3.1" << std::endl;
        double metric{0};
        metric += (p1.second * static_cast<double>(sizeOfUnionOfAllThreadsInSet(p1.first)));
        metric += (p2.second * static_cast<double>(sizeOfUnionOfAllThreadsInSet(p2.first)));
        for(auto it1 = p1.first.begin(); it1 != p1.first.end(); ++it1){
            for(auto it2 = p2.first.begin(); it2 != p2.first.end(); ++it2){
                metric += sizeOfIntersectionOfTwoPageSets(*it1, *it2);
            }
        }
        std::set<int> temp_set{p1.first};
        temp_set.insert(p2.first.begin(), p2.first.end());
        metric /= sizeOfUnionOfAllThreadsInSet(temp_set);
        return metric;
    }

    std::set<int> mergeSets(const std::set<int>& s1, const std::set<int> s2) const{
        std::set<int> mergedSet{s1};
        for(auto c: s2) mergedSet.insert(c);
        return mergedSet;
    }
public:
    GreedySchedulingAlgorithm(std::function<double(const PlacementStrategy&)> input_metric): metric(input_metric){}
    GreedySchedulingAlgorithm(const GreedySchedulingAlgorithm& other) = default;
    GreedySchedulingAlgorithm(GreedySchedulingAlgorithm&& other) = default; 
    GreedySchedulingAlgorithm& operator=(const GreedySchedulingAlgorithm& other) = default;
    GreedySchedulingAlgorithm& operator=(GreedySchedulingAlgorithm&& other) = delete;

    const PlacementStrategy& runAlgorithm(const Input* input_, int n_CN) const{
        input = input_;
        std::set<std::pair<std::set<int>, double>> current_thread_allocation;
        for(int i = 0; i < static_cast<int>(input->numberOfThreads()); ++i) current_thread_allocation.insert({std::set<int>{i}, 1});
        // for(auto it = current_thread_allocation.begin(); it != current_thread_allocation.end(); ++it){
        //     for(auto c: (*it).first) std::cerr << c << " , ";
        //     std::cerr << std::endl;
        // }
        while(current_thread_allocation.size() > n_CN){
            // std::cerr << "Checkpoint 1" << std::endl;
            double max_delta = -1000;
            double new_metric = -1000;
            // double new_metric = std::numeric_limits<double>::min();
            // std::cout << "MAX _DELTA:\t" << max_delta << std::endl;
            std::pair<decltype(current_thread_allocation.begin()), decltype(current_thread_allocation.begin())> thread_sets_to_be_merged {nullptr, nullptr};
            for(auto it1 = current_thread_allocation.begin(); it1 != current_thread_allocation.end(); ++it1){
                auto it2 = it1; ++it2;
                for(; it2 != current_thread_allocation.end(); ++it2){
                    new_metric = calculateMetricForSetCombination(*it1, *it2);
                    double current_delta = new_metric - (*it1).second - (*it2).second;
                    // std::cerr << "Current Delta:\t" << current_delta << std::endl;
                    if(current_delta > max_delta){
                        max_delta = current_delta;
                        thread_sets_to_be_merged = {it1,it2};
                    }
                }
                // std::cerr << "Current value of max_delta:\t" << max_delta << std::endl;
            }
            std::set<int> merged_thread_set{(*(thread_sets_to_be_merged.first)).first};
            merged_thread_set.insert(((*(thread_sets_to_be_merged.second)).first).begin(), ((*(thread_sets_to_be_merged.second)).first).end());
            std::pair<std::set<int>, double> new_merged_set{merged_thread_set, new_metric};
            current_thread_allocation.insert(new_merged_set);
            current_thread_allocation.erase(thread_sets_to_be_merged.first);
            current_thread_allocation.erase(thread_sets_to_be_merged.second);
        }
        // std::cerr << "Checkpoint x->1" << std::endl;
        PlacementStrategy* placement_of_threads = new PlacementStrategy(input, n_CN);
        int i = 0;
        for(auto it = current_thread_allocation.begin(); it != current_thread_allocation.end(); ++it, ++i){
            for(auto c: (*it).first) (placement_of_threads->assignment_of_CN)[c] = i;
            // for(auto c: (*it).first) std::cerr << c << " , ";
            // std::cerr << std::endl;
        }
        // std::cerr << "BEGION\n";
        // for(auto it = current_thread_allocation.begin(); it != current_thread_allocation.end(); ++it, ++i){
        //     std::cerr << (*it).second << ", ";
        // }
        // std::cerr << std::endl;
        return *placement_of_threads;
    }

    virtual ~GreedySchedulingAlgorithm() = default;
};

#endif