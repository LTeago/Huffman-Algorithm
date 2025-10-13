#ifndef FREQ_COUNTER_HPP
#define FREQ_COUNTER_HPP

#include <string>
#include <unordered_map>
#include "parser.hpp"

class FreqCounter {
public:
    void add(const std::string& token);
    void add(const Token& token);
    void print() const;

private:
    std::unordered_map<std::string, int> freq;
};

#endif
