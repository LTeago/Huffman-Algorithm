#include "parser.hpp"
#include "count-freq.hpp"
#include "huffman_node.h"
#include "bit.hpp"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <string>

using namespace std;

// ======= Função: gerar códigos de Huffman =======
void generateCodes(HuffmanNode* root, const string& prefix, unordered_map<string, string>& codes) {
    if (!root) return;
    if (!root->left && !root->right)
        codes[root->symbol] = prefix.empty() ? "0" : prefix;
    else {
        generateCodes(root->left, prefix + "0", codes);
        generateCodes(root->right, prefix + "1", codes);
    }
}

// ======= Compressão =======
void compressFile(const string& inputFile, const string& outputFile,
                  const unordered_map<string, string>& codes) {
    ifstream in(inputFile);
    if (!in.is_open()) throw runtime_error("Erro ao abrir arquivo de entrada");

    std::ofstream out(outputFile, std::ios::binary);
if (!out.is_open()) throw std::runtime_error("Erro ao criar arquivo de saída");
BitWriter writer(out);

    string word;
    char c;

    while (in.get(c)) {
        string s(1, c);
        if (codes.find(s) != codes.end())
    writer.write_bits(codes.at(s));
else if (codes.find("CHAR:" + s) != codes.end())
    writer.write_bits(codes.at("CHAR:" + s));
    }

    writer.flush();
    cout << "Arquivo comprimido salvo em: " << outputFile << "\n";
}

// ======= Descompressão =======
void decompressFile(const std::string& inputFile, const std::string& outputFile, HuffmanNode* root) {
    std::ifstream in(inputFile, std::ios::binary);
    if (!in.is_open()) throw std::runtime_error("Erro ao abrir arquivo comprimido");

    BitReader reader(in);
    std::ofstream out(outputFile);
    if (!out.is_open()) throw std::runtime_error("Erro ao criar arquivo de saída");

    HuffmanNode* node = root;
    int bit;
    std::string lastType = "";

    auto writeToken = [&](const std::string& sym) {
        if (sym.rfind("CHAR:", 0) == 0) {
            out << sym.substr(5);
            lastType = "CHAR";
        } 
        else if (sym.rfind("SYMBOL:", 0) == 0) {
            std::string s = sym.substr(7);
            out << s;
            if (s == "{" || s == "}" || s == ";")
                out << "\n";
            lastType = "SYMBOL";
        } 
        else if (sym.rfind("KEYWORD:", 0) == 0) {
            if (lastType != "SYMBOL" && lastType != "CHAR")
                out << " ";
            out << sym.substr(8);
            lastType = "KEYWORD";
        } 
        else if (sym.rfind("IDENTIFIER:", 0) == 0) {
            out << " " << sym.substr(12);
            lastType = "IDENTIFIER";
        } 
        else if (sym.rfind("NUMBER:", 0) == 0) {
            out << " " << sym.substr(7);
            lastType = "NUMBER";
        } 
        else if (sym.rfind("STRING:", 0) == 0) {
            out << " " << sym.substr(7);
            lastType = "STRING";
        } 
        else {
            out << sym;
            lastType = "";
        }
    };

    while ((bit = reader.read_bit()) != -1) {
        node = bit ? node->right : node->left;
        if (!node->left && !node->right) {
            writeToken(node->symbol);
            node = root;
        }
    }

    std::cout << "Arquivo descomprimido salvo em: " << outputFile << "\n";
}


// ======= MAIN =======
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Uso:\n"
             << argv[0] << " freq <arquivo>\n"
             << argv[0] << " c <entrada> <saida>\n"
             << argv[0] << " d <entrada.huff> <saida>\n";
        return 1;
    }

    string mode = argv[1];
    string input = argv[2];

    try {
        if (mode == "freq") {
            auto tokens = parse_tokens(input);
            FreqCounter counter;
            for (const auto& t : tokens)
                counter.add(t);
            counter.saveCSV("frequencias.csv");
            cout << "frequencias.csv gerado com sucesso!\n";
        }
        else if (mode == "c") {
            if (argc < 4) throw runtime_error("Faltando arquivo de saída");
            string output = argv[3];

            // Usa tokens para contar frequências e gerar códigos
            auto tokens = parse_tokens(input);
            FreqCounter counter;
            for (const auto& t : tokens)
                counter.add(t);

            auto freqMap = counter.getTable();
            HuffmanNode* root = buildHuffmanTree(freqMap);

            unordered_map<string, string> codes;
            generateCodes(root, "", codes);
            compressFile(input, output, codes);
            freeTree(root);
        }
        else if (mode == "d") {
            if (argc < 4) throw runtime_error("Faltando arquivo de saída");
            string output = argv[3];

            // Lê o mapa de frequências gerado antes
            ifstream csv("frequencias.csv");
            if (!csv.is_open()) throw runtime_error("frequencias.csv não encontrado");

            unordered_map<string, uint64_t> freqMap;
            string line;
            getline(csv, line); // ignora cabeçalho
            while (getline(csv, line)) {
                size_t comma = line.find_last_of(',');
                if (comma != string::npos) {
                    string token = line.substr(1, comma - 2); // remove aspas
                    uint64_t freq = stoull(line.substr(comma + 1));
                    freqMap[token] = freq;
                }
            }

            HuffmanNode* root = buildHuffmanTree(freqMap);
            decompressFile(input, output, root);
            freeTree(root);
        }
        else {
            cerr << "Modo inválido. Use freq, c ou d.\n";
        }
    } catch (const exception& e) {
        cerr << "Erro: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
