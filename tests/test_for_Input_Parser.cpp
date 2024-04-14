#include <iostream>
#include "../inputParsers.h"
#include "../input.h"
#include "../thread.h"

int main() {
    // TextInputParser t_parser("test_input.txt");
    // const Input* t_input_ptr = t_parser.getInputPtr();
    // for(const auto c: t_input_ptr->threads){
    //     for(auto d: (c->pages).pages){
    //         std::cout << d << " ";
    //     }
    //     std::cout << std::endl;
    // }

    RandomInputParser r_parser(5, 3, 100);
    const Input* t_input_ptr = r_parser.getInputPtr();
    for(const auto c: t_input_ptr->threads){
        for(auto d: (c->pages).pages){
            std::cout << d << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
