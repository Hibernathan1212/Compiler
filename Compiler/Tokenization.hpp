//
//  Tokenization.hpp
//  Compiler
//
//  Created by Nathan Thurber on 29/6/24.
//

#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <optional>
#include <string>
#include <vector>

enum class TokenType
{
    exit,
    int_lit,
    semi,
    open_paren,
    close_paren,
    ident,
    let,
    eq,
    plus,
    star,
    minus,
    fslash,
    open_curly,
    close_curly,
    if_,
    elif,
    else_
};

struct Token
{
    TokenType type;
    int line;
    std::optional<std::string> value {};
};

class Tokenizer
{
public:
    Tokenizer(const std::string src);
    
    std::vector<Token> tokenize();

private:
    [[nodiscard]] std::optional<char> peek(int offset = 0) const;
    char consume();
    
    const std::string m_src;
    size_t m_index = 0;
};

inline std::string to_string(TokenType type)
{
    switch (type) {
        case TokenType::exit:
            return "'exit'";
        case TokenType::int_lit:
            return "integer literal";
        case TokenType::semi:
            return "';'";
        case TokenType::open_paren:
            return "'('";
        case TokenType::close_paren:
            return "')'";
        case TokenType::ident:
            return "identifier";
        case TokenType::let:
            return "'let'";
        case TokenType::eq:
            return "'='";
        case TokenType::plus:
            return "'+'";
        case TokenType::star:
            return "'*'";
        case TokenType::minus:
            return "'-'";
        case TokenType::fslash:
            return "'/'";
        case TokenType::open_curly:
            return "'{'";
        case TokenType::close_curly:
            return "'}'";
        case TokenType::if_:
            return "'if'";
        case TokenType::elif:
            return "'elif'";
        case TokenType::else_:
            return "'else'";
        default:
            throw std::runtime_error("");
    }
}
