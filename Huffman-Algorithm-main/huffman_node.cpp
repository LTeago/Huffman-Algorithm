#include "huffman_node.h"
#include <queue>
#include <unordered_map>
#include <cstdint>
#include <vector>
#include <algorithm>

HuffmanNode* buildHuffmanTree(const std::unordered_map<std::string, uint64_t>& freqMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> pq;

    // Copia as entradas e ordena por símbolo (ordem determinística)
    std::vector<std::pair<std::string, uint64_t>> entries(freqMap.begin(), freqMap.end());
    std::sort(entries.begin(), entries.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });

    for (const auto& [symbol, freq] : entries)
        pq.push(new HuffmanNode(symbol, freq));

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();

        uint64_t combinedFreq = left->freq + right->freq;
        pq.push(new HuffmanNode("", combinedFreq, left, right));
    }

    return pq.empty() ? nullptr : pq.top();
}

void freeTree(HuffmanNode* node) {
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}
