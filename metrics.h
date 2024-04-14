#ifndef METRICS_H
#define METRICS_H

#include <set>
#include "simulator.h"
#include "thread.h"
#include "pageSet.h"


//conatins helper functions for all the metrics
namespace Metrics{
    int sizeOfUnionOfVectorOfPageSets(const std::vector<const PageSet*> input_pages){
        if(input_pages.size()==0) return 0;
        if(input_pages.size()==1) return input_pages[0]->size();
        std::set<int> unionSet;
        for(auto pageSet: input_pages){
            for(auto page: pageSet->pages){
                unionSet.insert(page);
            }
        }
        return unionSet.size();
    }

    int sizeOfIntersectionOfTwoPageSets(const PageSet* page1, const PageSet* page2){
        int metric_value{0};
        for(auto c: page1->pages) if((page2->pages).find(c)!=(page2->pages).end()) ++metric_value;
        return metric_value;
    }
}

double metric1(const PlacementStrategy& placement){
    const std::vector<std::vector<Thread*>> sets = placement.getCN_wise_thread_allocation();
    double metric_value = 0;
    for(auto& threads_on_a_CN: sets){
        std::vector<const PageSet*> pages_on_a_CN;
        for(auto thread_ptr: threads_on_a_CN){
            pages_on_a_CN.emplace_back(&(thread_ptr->pages));
        }
        metric_value += Metrics::sizeOfUnionOfVectorOfPageSets(pages_on_a_CN);
    }
    return -(metric_value/double(sets.size()));
}

double metric2(const PlacementStrategy& placement){
    const std::vector<std::vector<Thread*>> sets = placement.getCN_wise_thread_allocation();
    double metric_value = 0;
    for(auto& threads_on_a_CN: sets){
        if(threads_on_a_CN.size()==0) continue;
        double metric_value_for_the_given_CN {0};
        std::vector<const PageSet*> pages_on_a_CN;
        for(auto thread_ptr: threads_on_a_CN){
            pages_on_a_CN.emplace_back(&(thread_ptr->pages));
        }
        for(int i = 0; i < static_cast<int>(threads_on_a_CN.size()); ++i){
            for(int j = 0; j < static_cast<int>(threads_on_a_CN.size()); ++j){
                metric_value_for_the_given_CN += Metrics::sizeOfIntersectionOfTwoPageSets(pages_on_a_CN[i], pages_on_a_CN[j]);
            }
        }
        metric_value_for_the_given_CN /= (double(Metrics::sizeOfUnionOfVectorOfPageSets(pages_on_a_CN)));
        metric_value += metric_value_for_the_given_CN;
    }
    return metric_value;
}


#endif