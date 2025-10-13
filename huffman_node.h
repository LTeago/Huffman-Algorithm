#ifndef HUFFMAN_NODE_H
#define HUFFMAN_NODE_H

#include <string>

struct HuffmanNode {
    std::string symbol;  // agora é string, não char
    int freq;
    HuffmanNode *left;
    HuffmanNode *right;

    HuffmanNode(const std::string& s, int f, HuffmanNode* l = nullptr, HuffmanNode* r = nullptr)
        : symbol(s), freq(f), left(l), right(r) {}
};

struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) const {
        return a->freq > b->freq;
    }
};

#endif

