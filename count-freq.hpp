#ifndef FREQ_COUNTER_HPP
#define FREQ_COUNTER_HPP

#include <string>
#include <unordered_map>
#include "parser.hpp"
#include <cstdint>


class FreqCounter {
public:
    void add(const std::string& token);
    void add(const Token& token);
    void print() const;

    const std::unordered_map<std::string, uint64_t>& getTable() const { return freq; }

private:
    std::unordered_map<std::string, uint64_t> freq;
};

#endif
