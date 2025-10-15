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
    for (const auto& [token, count] : freq) {
        std::string safe = token;
        // substitui quebra de linha por literal \n
        size_t pos = 0;
        while ((pos = safe.find('\n', pos)) != std::string::npos) {
            safe.replace(pos, 1, "\\n");
            pos += 2;
        }
        // escapa aspas duplas
        pos = 0;
        while ((pos = safe.find('"', pos)) != std::string::npos) {
            safe.replace(pos, 1, "\"\"");
            pos += 2;
        }
        out << "\"" << safe << "\"," << count << "\n";
    }
}
