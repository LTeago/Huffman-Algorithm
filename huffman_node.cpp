#include "huffman_node.h"
#include <queue>
#include <unordered_map>

HuffmanNode* buildHuffmanTree(const std::unordered_map<std::string, int>& freqMap) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> pq;

    for (auto [symbol, freq] : freqMap)
        pq.push(new HuffmanNode(symbol, freq));

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();

        int combinedFreq = left->freq + right->freq;
        pq.push(new HuffmanNode("", combinedFreq, left, right));
    }

    return pq.top(); 
}
