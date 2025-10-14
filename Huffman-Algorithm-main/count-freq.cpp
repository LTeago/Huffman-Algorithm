#include "count-freq.hpp"
#include <iostream>
#include <fstream>

void FreqCounter::add(const std::string& token) {
    freq[token]++;
}

void FreqCounter::add(const Token& token) {
    freq[token.type + ":" + token.value]++;
}

void FreqCounter::print() const {
    for (const auto& [token, count] : freq)
        std::cout << token << ": " << count << '\n';
}

void FreqCounter::saveCSV(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) throw std::runtime_error("Erro ao criar arquivo CSV");

    out << "simbolo,frequencia\n";
    for (const auto& [token, count] : freq)
        out << "\"" << token << "\"," << count << "\n";
}
