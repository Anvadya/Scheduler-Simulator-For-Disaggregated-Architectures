#ifndef INPUT_H
#define INPUT_H

#include <cstddef>
#include <vector>
#include "thread.h"

class Input{
private:

public:
    const std::vector<Thread*> threads;
    
    Input(std::vector<Thread*> input_thread_vector):threads(std::move(input_thread_vector)){}

    ~Input(){
        for(auto obj_to_be_deleted: threads) if(obj_to_be_deleted) delete obj_to_be_deleted;
    }
    
    std::size_t numberOfThreads() const {return threads.size();}
    const std::set<int>* pages_in_PageSet(int i) const{
        if(i<0 || i>=numberOfThreads()) return nullptr;
        return &(threads[i]->pages.pages);
    }
};

#endif