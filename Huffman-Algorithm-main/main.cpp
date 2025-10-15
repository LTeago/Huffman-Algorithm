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
    
    auto tokens = parse_tokens(inputFile);

    ofstream out(outputFile, ios::binary);
    if (!out.is_open()) throw runtime_error("Erro ao criar arquivo de saída");

    BitWriter writer(out);

    size_t written = 0;
    for (const auto& token : tokens) {
        string key = token.type + ":" + token.value;
        auto it = codes.find(key);
        if (it == codes.end()) {
            cerr << "Aviso: simbolo nao encontrado ao comprimir: " << key << "\n";
            continue;
        }
        writer.write_bits(it->second);
        written++;
    }

    writer.flush();
    cout << "Arquivo comprimido salvo em: " << outputFile; 

}

// ======= Descompressão =======
void decompressFile(const string& inputFile, const string& outputFile, HuffmanNode* root) {
    ifstream in(inputFile, ios::binary);
    if (!in.is_open()) throw runtime_error("Erro ao abrir arquivo comprimido");

    BitReader reader(in);
    ofstream out(outputFile);
    if (!out.is_open()) throw runtime_error("Erro ao criar arquivo de saida");

    HuffmanNode* node = root;
    int bit;
    size_t decoded = 0;
    size_t safetyCounter = 0;

    while ((bit = reader.read_bit()) != -1) {
        if (!node) {
            cerr << "[ERRO] No nulo encontrado — arvore inconsistente!\n";
            break;
        }

        node = bit ? node->right : node->left;

        // Falha se o bit stream não corresponde a nenhum nó válido
        if (!node) {
            cerr << "[ERRO] Caminho invalido na arvore de Huffman. Bit=" << bit << "\n";
            break;
        }

        // Chegou a uma folha → imprime símbolo
        if (!node->left && !node->right) {
            string sym = node->symbol;
            size_t sep = sym.find(':');
            string type, value;

            if (sep != string::npos) {
                type = sym.substr(0, sep);
                value = sym.substr(sep + 1);
            } else {
                value = sym;
            }

            if (type == "SPACE") {
                out << value;
            }
            else if (type == "NEWLINE") {
                out << '\n';
            }
            else {
                out << value;
            }

            node = root;
            decoded++;
        }

        // seguranca: evita loops infinitos
        if (++safetyCounter > 50'000'000) {
            cerr << "[ERRO] Loop detectado após 50 milhões de bits — abortando.\n";
            break;
        }
    }

    cout << "Descompressao concluida." << "\n";
    cout << "Arquivo salvo em: " << outputFile << "\n";
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
            if (argc < 4) throw runtime_error("Faltando arquivo de saida");
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
            if (argc < 4) throw runtime_error("Faltando arquivo de saida");
            string output = argv[3];

            // Lê o mapa de frequências gerado antes
            ifstream csv("frequencias.csv");
            if (!csv.is_open()) throw runtime_error("frequencias.csv nao encontrado");

         
unordered_map<string, uint64_t> freqMap;
string line;
getline(csv, line); // ignora cabeçalho

while (getline(csv, line)) {
    if (line.empty()) continue;

    size_t comma = line.rfind(',');
    if (comma == string::npos) continue;

    string token = line.substr(0, comma);
    uint64_t freq = 0;
    try {
        freq = stoull(line.substr(comma + 1));
    } catch (...) {
        continue;
    }

    // remove aspas externas
    if (token.size() >= 2 && token.front() == '"' && token.back() == '"')
        token = token.substr(1, token.size() - 2);

    // restaura aspas duplas escapadas
    size_t pos = 0;
    while ((pos = token.find("\"\"", pos)) != string::npos) {
        token.replace(pos, 2, "\"");
        pos += 1;
    }

    // restaura quebras de linha literais
    pos = 0;
    while ((pos = token.find("\\n", pos)) != string::npos) {
        token.replace(pos, 2, "\n");
        pos += 1;
    }

    freqMap[token] = freq;
}


            HuffmanNode* root = buildHuffmanTree(freqMap);
            decompressFile(input, output, root);
            freeTree(root);
        }

        else {
            cerr << "Modo invalido. Use freq, c ou d.\n";
        }
    }
    catch (const exception& e) {
        cerr << "Erro: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
