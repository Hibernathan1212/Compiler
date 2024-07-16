//
//  Tokenization.cpp
//  Compiler
//
//  Created by Nathan Thurber on 29/6/24.
//

#include "Tokenization.hpp"

Tokenizer::Tokenizer(const std::string src)
    : m_src(std::move(src)) {}

std::vector<Token> Tokenizer::tokenize()
{
    std::vector<Token> Tokens;
    std::string buf;
    int line_count = 1;
    
    while(peek().has_value())
    {
        if (isalpha(peek().value()))
        {
            buf.push_back(consume());
            while (peek().has_value() && isalpha(peek().value()))
            {
                buf.push_back(consume());
            }
            if (buf == "exit")
            {
                Tokens.push_back({ TokenType::exit, line_count });
                buf.clear();
            }
            else if (buf == "let")
            {
                Tokens.push_back({ TokenType::let, line_count });
                buf.clear();
            }
            else if (buf == "if")
            {
                Tokens.push_back({ TokenType::if_, line_count });
                buf.clear();
            }
            else if (buf == "elif")
            {
                Tokens.push_back({ TokenType::elif, line_count });
                buf.clear();
            }
            else if (buf == "else")
            {
                Tokens.push_back({ TokenType::else_, line_count });
                buf.clear();
            }
            else
            {
                Tokens.push_back({ TokenType::ident, line_count, buf });
                buf.clear();
            }
        }
        else if (isdigit(peek().value()))
        {
            buf.push_back(consume());
            while (peek().has_value() && isdigit(peek().value()))
            {
                buf.push_back(consume());
            }
            
            Tokens.push_back({ TokenType::int_lit, line_count, buf });
            buf.clear();
        }
        else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '/')
        {
            consume();
            consume();
            while (peek().has_value() && peek().value() != '\n')
            {
                consume();
            }
        }
        else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '*')
        {
            consume();
            consume();
            while (peek().has_value())
            {
                if (peek().value() == '*' && peek(1).has_value() && peek(1).value() == '/')
                {
                    break;
                }
                if (peek().value() == '\n')
                {
                    line_count++;
                }
                consume();
            }
            if (peek().has_value())
            {
                consume();
            }
            consume();
        }
        else if (peek().value() == '(')
        {
            consume();
            Tokens.push_back({ TokenType::open_paren, line_count });
        }
        else if (peek().value() == ')')
        {
            consume();
            Tokens.push_back({ TokenType::close_paren, line_count });
        }
        else if (peek().value() == ';')
        {
            consume();
            Tokens.push_back({ TokenType::semi, line_count });
        }
        else if (peek().value() == '=')
        {
            consume();
            Tokens.push_back({ TokenType::eq, line_count });
        }
        else if (peek().value() == '+')
        {
            consume();
            Tokens.push_back({ TokenType::plus, line_count });
        }
        else if (peek().value() == '*')
        {
            consume();
            Tokens.push_back({ TokenType::star, line_count });
        }
        else if (peek().value() == '-')
        {
            consume();
            Tokens.push_back({ TokenType::minus, line_count });
        }
        else if (peek().value() == '/')
        {
            consume();
            Tokens.push_back({ TokenType::fslash, line_count });
        }
        else if (peek().value() == '{')
        {
            consume();
            Tokens.push_back({ TokenType::open_curly, line_count });
        }
        else if (peek().value() == '}')
        {
            consume();
            Tokens.push_back({ TokenType::close_curly, line_count });
        }
        else if (peek().value() == '\n')
        {
            consume();
            line_count++;
        }
        else if (isspace(peek().value()))
        {
            consume();
        }
        else
        {
            std::cerr << "Invalid token" << std::endl;
        }
    }
    
    m_index = 0;
    return Tokens;
}

std::optional<char> Tokenizer::peek(int offset) const
{
    if (m_index + offset >= m_src.length())
    {
        return {};
    }
    else
    {
        return m_src.at(m_index + offset);
    }
}

char Tokenizer::consume()
{
    char c = m_src.at(m_index);
    m_index++;
    return c;
}
