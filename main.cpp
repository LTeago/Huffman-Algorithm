#include "count-freq.hpp"
#include "parser.hpp"
#include "huffman_node.h"
#include <iostream>
#include <unordered_map>
#include <bitset>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

void generateCodes(HuffmanNode *root, const std::string &prefix, std::unordered_map<std::string, std::string> &codes)
{
    if (!root) return;
    if (!root->left && !root->right) {
        codes[root->symbol] = prefix.empty() ? "0" : prefix;
        return;
    }
    generateCodes(root->left, prefix + "0", codes);
    generateCodes(root->right, prefix + "1", codes);
}

void compress_file(const std::string &inputFile, const std::string &outputFile)
{
    std::ifstream in(inputFile, std::ios::binary);
    if (!in) {
        std::cerr << "Erro ao abrir arquivo de entrada.\n";
        return;
    }

    std::unordered_map<std::string, uint64_t> freq;
    char c;
    while (in.get(c)) {
        std::string s(1, c);
        freq[s]++;
    }

    if (freq.empty()) {
        std::cerr << "Arquivo vazio — nada a comprimir.\n";
        return;
    }

    in.clear();
    in.seekg(0);

    HuffmanNode *root = buildHuffmanTree(freq);
    if (!root) {
        std::cerr << "Erro ao construir árvore de Huffman.\n";
        return;
    }

    std::unordered_map<std::string, std::string> codes;
    generateCodes(root, "", codes);

    std::string bitString;
    while (in.get(c)) {
        std::string key(1, c);
        bitString += codes.at(key);
    }

    std::ofstream out(outputFile, std::ios::binary);
    if (!out) {
        std::cerr << "Erro ao abrir arquivo de saída.\n";
        freeTree(root);
        return;
    }

    uint64_t mapSize = freq.size();
    out.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));
    for (const auto &kv : freq) {
        const std::string &sym = kv.first;
        uint64_t symLen = static_cast<uint64_t>(sym.size());
        out.write(reinterpret_cast<const char*>(&symLen), sizeof(symLen));
        out.write(sym.data(), static_cast<std::streamsize>(symLen));
        out.write(reinterpret_cast<const char*>(&kv.second), sizeof(kv.second));
    }

    uint64_t totalBits = bitString.size();
    out.write(reinterpret_cast<const char*>(&totalBits), sizeof(totalBits));

    uint8_t buffer = 0;
    int bitCount = 0;
    for (char bit : bitString) {
        buffer = static_cast<uint8_t>((buffer << 1) | (bit == '1' ? 1u : 0u));
        bitCount++;
        if (bitCount == 8) {
            out.put(static_cast<char>(buffer));
            buffer = 0;
            bitCount = 0;
        }
    }
    if (bitCount > 0) {
        buffer <<= (8 - bitCount);
        out.put(static_cast<char>(buffer));
    }

    freeTree(root);
    std::cout << "Arquivo comprimido com sucesso em " << outputFile << "\n";
}

void decompress_file(const std::string &inputFile, const std::string &outputFile)
{
    std::ifstream in(inputFile, std::ios::binary);
    if (!in) {
        std::cerr << "Erro ao abrir arquivo comprimido.\n";
        return;
    }

    uint64_t mapSize = 0;
    in.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));
    if (!in) { std::cerr << "Erro lendo cabeçalho (mapSize).\n"; return; }

    std::unordered_map<std::string, uint64_t> freq;
    for (uint64_t i = 0; i < mapSize; ++i) {
        uint64_t symLen = 0;
        in.read(reinterpret_cast<char*>(&symLen), sizeof(symLen));
        if (!in) { std::cerr << "Erro lendo symLen.\n"; return; }

        std::string sym(symLen, '\0');
        in.read(sym.data(), static_cast<std::streamsize>(symLen));
        if (!in) { std::cerr << "Erro lendo símbolo.\n"; return; }

        uint64_t f = 0;
        in.read(reinterpret_cast<char*>(&f), sizeof(f));
        if (!in) { std::cerr << "Erro lendo freq.\n"; return; }

        freq[sym] = f;
    }

    uint64_t totalBits = 0;
    in.read(reinterpret_cast<char*>(&totalBits), sizeof(totalBits));
    if (!in) { std::cerr << "Erro lendo totalBits.\n"; return; }

    HuffmanNode *root = buildHuffmanTree(freq);
    if (!root) { std::cerr << "Erro ao construir árvore de Huffman.\n"; return; }

    std::ofstream out(outputFile, std::ios::binary);
    if (!out) { std::cerr << "Erro ao abrir arquivo de saída.\n"; freeTree(root); return; }

    HuffmanNode* node = root;
    uint64_t bitsConsumed = 0;
    char byteChar;
    while (bitsConsumed < totalBits && in.get(byteChar)) {
        uint8_t byte = static_cast<uint8_t>(byteChar);
        for (int i = 7; i >= 0 && bitsConsumed < totalBits; --i) {
            bool bit = ((byte >> i) & 1u) != 0;
            node = bit ? node->right : node->left;
            if (!node) {
                std::cerr << "Erro: percurso inválido na árvore (bit stream corrupto).\n";
                freeTree(root);
                return;
            }
            if (!node->left && !node->right) {
                out.write(node->symbol.data(), static_cast<std::streamsize>(node->symbol.size()));
                node = root;
            }
            bitsConsumed++;
        }
    }

    freeTree(root);
    std::cout << "Arquivo descomprimido com sucesso em " << outputFile << "\n";
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " -c|-d <arquivo>\n";
        return 1;
    }

    for (const auto& [symbol, code] : code)
        std::cout << symbol << " -> " << code << "\n";
    std::string mode = argv[1];
    std::string filename = argv[2];

    if (mode == "-c") {
        compress_file(filename, filename + ".huf");
    } else if (mode == "-d") {
        decompress_file(filename, filename + ".dec");
    } else {
        std::cerr << "Modo desconhecido: use -c (compactar) ou -d (descompactar)\n";
        return 1;
    }
    return 0;
}
