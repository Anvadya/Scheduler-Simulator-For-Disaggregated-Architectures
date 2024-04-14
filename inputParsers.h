#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <string>
#include <fstream>
#include <set>
#include <vector>
#include <random>
#include <chrono>
#include "thread.h"
#include "input.h"

class InputParser{
private:
    virtual bool parseInput() = 0;
protected:
    Input* input = nullptr;
public:
    const Input* getInputPtr() const{
        return input;
    }
};

class TextInputParser : public InputParser {
private:
    std::string nameOfInputFile;
    std::vector<Thread*> thread_vector;

    virtual bool parseInput(){
        std::ifstream inputFile(nameOfInputFile);


        int n_threads;
        inputFile >> n_threads;

        for(int i = 0; i < n_threads; ++i){
            std::set<int> tmpPages;
            int n_pages;
            inputFile >> n_pages;
            for(int j = 0; j < n_pages; ++j){
                int pgNumber; inputFile >> pgNumber;
                tmpPages.insert(pgNumber);
            }
            Thread* tmpThread = new Thread(tmpPages);
            thread_vector.emplace_back(tmpThread);
        }

        input = new Input(thread_vector);

        return true;
    }
public:
    TextInputParser():nameOfInputFile("input.txt"){parseInput();}
    TextInputParser(std::string inputFile):nameOfInputFile(inputFile){parseInput();}

    ~TextInputParser(){
        for(auto &thr: thread_vector) if(thr) {delete thr; thr = nullptr;}
        if(input) {delete input; input = nullptr;}
    }
};


class RandomInputParser: public InputParser{
private:
    const int n_threads;
    const int total_pages;
    const std::vector<int> pages_for_thread;
    std::vector<Thread*> thread_vector;

    static int getRandomNumber(int range_min, int range_max){
        static std::random_device rd;
        static int chaos = std::chrono::duration_cast<std::chrono::nanoseconds>((std::chrono::high_resolution_clock::now()).time_since_epoch()).count();
        static std::mt19937 gen(rd() + chaos); 
        std::uniform_int_distribution<int> dis(range_min, range_max);
        int rand_num = dis(gen);
        return rand_num;
    }

    virtual bool parseInput(){
        for(int i = 0; i < n_threads; ++i){
            std::set<int> pages_for_current_thread;
            while(int(pages_for_current_thread.size()) < pages_for_thread[i]){
                pages_for_current_thread.insert(getRandomNumber(0, total_pages-1));
            }
            Thread* thread_ptr = new Thread(pages_for_current_thread);
            thread_vector.emplace_back(thread_ptr);
        }
        input = new Input(thread_vector);
        return true;
    }
public:
    RandomInputParser(int input_n_threads, int n_pages, int size_of_memory)
        :n_threads(input_n_threads), total_pages(size_of_memory), pages_for_thread(input_n_threads, n_pages){parseInput();}

    ~RandomInputParser(){
        for(auto& c: thread_vector) if(c) {delete c; c = nullptr;}
        if(input) {delete input; input = nullptr;}
    }
};

#endif