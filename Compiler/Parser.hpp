//
//  Parser.hpp
//  Compiler
//
//  Created by Nathan Thurber on 30/6/24.
//

#pragma once

#include "Arena.hpp"
#include "Tokenization.hpp"
#include <variant>

struct NodeTermIntLit
{
    Token int_lit;
};

struct NodeTermIdent
{
    Token ident;
};

struct NodeExpr;

struct NodeBinExprAdd
{
    NodeExpr* lhs;
    NodeExpr* rhs;
};
struct NodeBinExprSub
{
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprMulti
{
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprDiv
{
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeTermParen
{
    NodeExpr* expr;
};

struct NodeBinExpr
{
    std::variant<NodeBinExprAdd*, NodeBinExprMulti*, NodeBinExprSub*, NodeBinExprDiv*> var;
};

struct NodeTerm
{
    std::variant<NodeTermIntLit*, NodeTermIdent*, NodeTermParen*> var;
};

struct NodeExpr
{
    std::variant<NodeTerm*, NodeBinExpr*> var;
};

struct NodeStmtLet
{
    Token ident;
    NodeExpr* expr;
};

struct NodeStmtExit
{
    NodeExpr* expr;
};

struct NodeStmt;

struct NodeScope
{
    std::vector<NodeStmt*> stmts;
};

struct NodeIfPred;

struct NodeStmtIf
{
    NodeExpr* expr;
    NodeScope* scope;
    std::optional<NodeIfPred*> pred;
};

struct NodeIfPredElif
{
    NodeExpr* expr;
    NodeScope* scope;
    std::optional<NodeIfPred*> pred;
};

struct NodeIfPredElse
{
    NodeScope* scope;
};

struct NodeIfPred
{
    std::variant<NodeIfPredElif*, NodeIfPredElse*> var;
};

struct NodeStmtAsign
{
    Token ident;
    NodeExpr* expr;
};

struct NodeStmt
{
    std::variant<NodeStmtExit*, NodeStmtLet*, NodeScope*, NodeStmtIf*, NodeStmtAsign*> var;
};

struct NodeProg
{
    std::vector<NodeStmt*> stmts;
};

class Parser
{
public:
    Parser(std::vector<Token> tokens);
    
    std::optional<NodeTerm*> parse_term();
    std::optional<NodeBinExpr*> parse_bin_expr();
    std::optional<NodeExpr*> parse_expr(int min_prec = 0);
    std::optional<NodeStmt*> parse_stmt();
    std::optional<NodeScope*> parse_scope();
    std::optional<NodeIfPred*> parse_if_pred();

    std::optional<NodeProg> parse_prog();

private:
    std::optional<Token> peek(int offset = 0) const;
    
    Token consume();
    Token try_consume_err(TokenType type);
    std::optional<Token> try_consume(TokenType type);
    
    [[noreturn]] const void error_expected(const std::string& msg);
    
    const std::vector<Token> m_tokens;
    size_t m_index = 0;
    
    ArenaAllocator m_allocator;
};
