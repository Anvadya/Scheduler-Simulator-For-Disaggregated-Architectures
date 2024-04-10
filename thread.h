#ifndef THREAD_H
#define THREAD_H

#include "pageSet.h"
#include "inputParsers.h"

class Thread{
private:

public:
    const PageSet pages;
    Thread(PageSet inputPageSet):pages(std::move(inputPageSet)){}
};

#endif