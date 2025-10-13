#include "count-freq.hpp"
#include "parser.hpp"
#include "huffman_node.h"
#include <iostream>
#include <unordered_map>

void generateCodes(HuffmanNode* root, const std::string& prefix, std::unordered_map<std::string, std::string>& codes) {
    if (!root) return;

    if (!root->left && !root->right) {
        codes[root->symbol] = prefix;
        return;
    }

    generateCodes(root->left, prefix + "0", codes);
    generateCodes(root->right, prefix + "1", codes);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo-fonte>\n";
        return 1;
    }

    // 1. Tokeniza
    auto tokens = parse_tokens(argv[1]);

    // 2. Conta frequências
    FreqCounter counter;
    for (const auto& token : tokens)
        counter.add(token);

    // 3. Constrói o mapa de frequências
    const auto& freqMap = counter.getTable();

    // 4. Cria árvore de Huffman
    HuffmanNode* root = buildHuffmanTree(freqMap);

    // 5. Gera códigos binários
    std::unordered_map<std::string, std::string> codes;
    generateCodes(root, "", codes);

    // 6. Mostra resultados
    for (const auto& [symbol, code] : codes)
        std::cout << symbol << " -> " << code << "\n";

    return 0;
}
