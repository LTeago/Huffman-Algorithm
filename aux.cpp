// huffman_codes.cpp
#include "huffman_node.h"
#include <unordered_map>
#include <string>

void generateCodes(HuffmanNode* root, const std::string& prefix, std::unordered_map<char, std::string>& codes) {
    if (!root) return;

    if (!root->left && !root->right) {
        codes[root->symbol] = prefix;
        return;
    }

    generateCodes(root->left, prefix + "0", codes);
    generateCodes(root->right, prefix + "1", codes);
}
