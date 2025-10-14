#ifndef HUFFMAN_NODE_H
#define HUFFMAN_NODE_H

#include <string>
#include <unordered_map>
#include <cstdint>

struct HuffmanNode {
    std::string symbol;  // pode ter mais de 1 byte
    uint64_t freq;
    HuffmanNode *left;
    HuffmanNode *right;

    HuffmanNode(const std::string& s, uint64_t f, HuffmanNode* l = nullptr, HuffmanNode* r = nullptr)
        : symbol(s), freq(f), left(l), right(r) {}
};

struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) const {
        return a->freq > b->freq;
    }
};

HuffmanNode* buildHuffmanTree(const std::unordered_map<std::string, uint64_t>& freqMap);
void freeTree(HuffmanNode* node);

#endif
