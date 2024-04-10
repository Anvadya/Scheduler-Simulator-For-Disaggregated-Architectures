#ifndef INPUT_H
#define INPUT_H

#include <cstddef>
#include <vector>
#include "thread.h"
#include "inputParsers.h"

class Input{
private:

public:
    const std::vector<Thread*> threads;
    
    Input(std::vector<Thread*> input_thread_vector):threads(std::move(input_thread_vector)){}

    ~Input(){
        for(auto obj_to_be_deleted: threads) if(obj_to_be_deleted) delete obj_to_be_deleted;
    }
    
    std::size_t numberOfThreads() const {return threads.size();}
};

#endif