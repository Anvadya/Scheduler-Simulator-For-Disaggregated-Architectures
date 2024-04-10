#ifndef PAGE_SET_H
#define PAGE_SET_H

#include<set>

class PageSet{
private:
public:
    const std::multiset<int> pages;

    PageSet(std::multiset<int> input):pages(std::move(input)){}
    PageSet(const PageSet& input) = default;
    PageSet(PageSet&& input) = default;

    std::size_t size() const{return pages.size();}
};

#endif