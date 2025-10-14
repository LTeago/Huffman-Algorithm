#include "count-freq.hpp"
#include <iostream>

void FreqCounter::add(const std::string& token) {
    freq[token]++;
}

void FreqCounter::add(const Token& token) {
    freq[token.type + ":" + token.value]++;
}

void FreqCounter::print() const {
    for (const auto& i : freq)
        std::cout << i.first << ": " << i.second << '\n';
}
