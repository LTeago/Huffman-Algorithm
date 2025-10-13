#include "count-freq.hpp"
#include "parser.hpp"
#include <iostream>
#include <unordered_map>
#include "huffman_node.h"

HuffmanNode* buildHuffmanTree(const std::unordered_map<char, int>& freqMap);

void generateCodes(HuffmanNode* root, const std::string& prefix, std::unordered_map<char, std::string>& codes) {
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

    auto tokens = parse_tokens(argv[1]);
    FreqCounter counter;

    for (const auto& token : tokens)
        counter.add(token);

    counter.print();
    return 0;
}
