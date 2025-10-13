#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

struct Token {
    std::string type;  
    std::string value;  
};

std::vector<Token> parse_tokens(const std::string& filename);

#endif
