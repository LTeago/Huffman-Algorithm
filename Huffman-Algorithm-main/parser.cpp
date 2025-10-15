#include "parser.hpp"
#include <fstream>
#include <sstream>
#include <cctype>
#include <unordered_set>

static const std::unordered_set<std::string> keywords = {
    "int", "float", "double", "bool", "char", "void", "if", "else", "for", "while",
    "return", "switch", "case", "break", "continue", "class", "struct", "public",
    "private", "protected", "true", "false", "include", "namespace", "using", "new",
    "delete", "this", "const", "static", "virtual", "template", "typename"
};

static const std::unordered_set<std::string> two_ops = {
    "==", "!=", "<=", ">=", "++", "--", "->", "+=", "-=", "*=", "/=", "&&", "||", "<<", ">>", "::"
};

static bool is_symbol(char c) {
    static const std::string symbols = "{}[]();,+-*/%=<>&|!^~.:#";
    return symbols.find(c) != std::string::npos;
}

std::vector<Token> parse_tokens(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<Token> tokens;
    if (!file.is_open()) return tokens;

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string code = ss.str();
    size_t i = 0;

    while (i < code.size()) {
        char c = code[i];

        // ======== Preserva espaços e quebras de linha ========
        if (c == ' ' || c == '\t') {
            tokens.push_back({"SPACE", " "});
            i++;
            continue;
        }
        if (c == '\n' || c == '\r') {
            tokens.push_back({"NEWLINE", "\n"});
            i++;
            continue;
        }

        // ======== Strings e chars literais ========
        if (c == '"' || c == '\'') {
            char quote = c;
            std::string lit; lit.push_back(quote); i++;
            while (i < code.size()) {
                char cur = code[i];
                lit.push_back(cur);
                if (cur == '\\' && i + 1 < code.size()) { i++; lit.push_back(code[i]); }
                else if (cur == quote) { i++; break; }
                i++;
            }
            tokens.push_back({"STRING", lit});
            continue;
        }

        // ======== Identificadores, palavras-chave e números ========
        if (isalnum(c) || c == '_') {
            size_t start = i;
            while (i < code.size() && (isalnum(code[i]) || code[i] == '_')) i++;
            std::string word = code.substr(start, i - start);

            if (keywords.count(word)) tokens.push_back({"KEYWORD", word});
            else if (isdigit(word[0])) tokens.push_back({"NUMBER", word});
            else tokens.push_back({"IDENTIFIER", word});

            continue;
        }

        // ======== Operadores de dois caracteres ========
        char next = (i + 1 < code.size()) ? code[i + 1] : '\0';
        std::string two; two.push_back(c); two.push_back(next);
        if (two_ops.count(two)) {
            tokens.push_back({"SYMBOL", two});
            i += 2;
            continue;
        }

        // ======== Símbolos de um caractere ========
        if (is_symbol(c)) {
            tokens.push_back({"SYMBOL", std::string(1, c)});
            i++;
            continue;
        }

        // ======== Qualquer outro caractere ========
        tokens.push_back({"CHAR", std::string(1, c)});
        i++;
    }

    return tokens;
}
