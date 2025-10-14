#ifndef FREQ_COUNTER_HPP
#define FREQ_COUNTER_HPP

#include <string>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <iostream>
#include "parser.hpp"

class FreqCounter {
public:
    void add(const std::string& token);
    void add(const Token& token);
    void print() const;
    void saveCSV(const std::string& filename) const;

    const std::unordered_map<std::string, uint64_t>& getTable() const { return freq; }

private:
    std::unordered_map<std::string, uint64_t> freq;
};

#endif
