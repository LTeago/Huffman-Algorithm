// huffman_node.h
#ifndef HUFFMAN_NODE_H
#define HUFFMAN_NODE_H

struct HuffmanNode {
    char symbol;
    int freq;
    HuffmanNode *left;
    HuffmanNode *right;

    HuffmanNode(char s, int f, HuffmanNode* l = nullptr, HuffmanNode* r = nullptr)
        : symbol(s), freq(f), left(l), right(r) {}
};


struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq; 
    }
};

#endif
