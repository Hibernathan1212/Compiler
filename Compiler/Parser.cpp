//
//  Parser.cpp
//  Compiler
//
//  Created by Nathan Thurber on 30/6/24.
//

#include "Parser.hpp"

std::optional<int> bin_prec(TokenType type)
{
    switch (type) 
    {
        case TokenType::plus:
        case TokenType::minus:
            return 0;
        case TokenType::star:
        case TokenType::fslash:
            return 1;
        default:
            return {};
    }
}

Parser::Parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens)), m_allocator(4 * 1024 * 1024) {}

std::optional<NodeTerm*> Parser::parse_term()
{
    if (auto int_lit = try_consume(TokenType::int_lit))
    {
        auto term_int_lit = m_allocator.alloc<NodeTermIntLit>();
        term_int_lit->int_lit = int_lit.value();
        auto term = m_allocator.alloc<NodeTerm>();
        term->var = term_int_lit;
        return term;
    }
    if (auto ident = try_consume(TokenType::ident))
    {
        auto term_ident = m_allocator.alloc<NodeTermIdent>();
        term_ident->ident = ident.value();
        auto term = m_allocator.alloc<NodeTerm>();
        term->var = term_ident;
        return term;
    }
    if (auto open_paren = try_consume(TokenType::open_paren))
    {
        auto expr = parse_expr();
        if (!expr.has_value())
        {
            error_expected("expression");
        }
        try_consume_err(TokenType::close_paren);
        auto term_paren = m_allocator.alloc<NodeTermParen>();
        term_paren->expr = expr.value();
        auto term = m_allocator.alloc<NodeTerm>();
        term->var = term_paren;
        return term;
    }
    else
    {
        return {};
    }
}

std::optional<NodeExpr*> Parser::parse_expr(int min_prec)
{
    std::optional<NodeTerm*> term_lhs = parse_term();
    if (!term_lhs.has_value())
    {
        return {};
    }
    
    auto expr_lhs = m_allocator.alloc<NodeExpr>();
    expr_lhs->var = term_lhs.value();
    
    while (true)
    {
        std::optional<Token> curr_tok = peek();
        std::optional<int> prec;
        if (curr_tok.has_value())
        {
            prec = bin_prec(curr_tok->type);
            if (!prec.has_value() || prec < min_prec)
            {
                break;
            }
        }
        else
        {
            break;
        }
        
        Token op = consume();
        int next_min_prec = prec.value() + 1;
        auto expr_rhs = parse_expr(next_min_prec);
        
        if (!expr_rhs.has_value())
        {
            error_expected("expression");
        }
        
        auto expr = m_allocator.alloc<NodeBinExpr>();
        auto expr_lhs2 = m_allocator.alloc<NodeExpr>();
        
        if (op.type == TokenType::plus)
        {
            auto add = m_allocator.alloc<NodeBinExprAdd>();
            expr_lhs2->var = expr_lhs->var;
            add->lhs = expr_lhs2;
            add->rhs = expr_rhs.value();
            expr->var = add;
            
        }
        else if (op.type == TokenType::star)
        {
            auto multi = m_allocator.alloc<NodeBinExprMulti>();
            expr_lhs2->var = expr_lhs->var;
            multi->lhs = expr_lhs2;
            multi->rhs = expr_rhs.value();
            expr->var = multi;
        }
        else if (op.type == TokenType::minus)
        {
            auto minus = m_allocator.alloc<NodeBinExprSub>();
            expr_lhs2->var = expr_lhs->var;
            minus->lhs = expr_lhs2;
            minus->rhs = expr_rhs.value();
            expr->var = minus;
        }
        else if (op.type == TokenType::fslash)
        {
            auto fslash = m_allocator.alloc<NodeBinExprDiv>();
            expr_lhs2->var = expr_lhs->var;
            fslash->lhs = expr_lhs2;
            fslash->rhs = expr_rhs.value();
            expr->var = fslash;
        }
        else
        {
            throw std::runtime_error("Unreachable");
        }
        expr_lhs->var = expr;
    }
    return expr_lhs;
}

std::optional<NodeScope*> Parser::parse_scope()
{
    if (!try_consume(TokenType::open_curly))
    {
        return {};
    }
    
    auto scope = m_allocator.alloc<NodeScope>();
    while (auto stmt = parse_stmt())
    {
        scope->stmts.push_back(stmt.value());
    }
    try_consume_err(TokenType::close_curly);
    return scope;
}

std::optional<NodeIfPred*> Parser::parse_if_pred()
{
    if (try_consume(TokenType::elif))
    {
        try_consume_err(TokenType::open_paren);
        auto elif = m_allocator.alloc<NodeIfPredElif>();
        if (auto expr = parse_expr())
        {
            elif->expr = expr.value();
        }
        else
        {
            error_expected("expression");
        }
        try_consume_err(TokenType::close_paren);
        if (auto scope = parse_scope())
        {
            elif->scope = scope.value();
        }
        else
        {
            error_expected("statement");
        }
        elif->pred = parse_if_pred();
        auto pred = m_allocator.alloc<NodeIfPred>();
        pred->var = elif;
        return pred;
    }
    if (try_consume(TokenType::else_))
    {
        auto else_ = m_allocator.alloc<NodeIfPredElse>();
        if (auto scope = parse_scope())
        {
            else_->scope = scope.value();
        }
        else
        {
            error_expected("statement");
        }
        auto pred = m_allocator.alloc<NodeIfPred>();
        pred->var = else_;
        return pred;
    }
    return {};
}

std::optional<NodeStmt*> Parser::parse_stmt()
{
    if (peek().has_value() && peek().value().type == TokenType::exit && peek(1).has_value()
        && peek(1).value().type == TokenType::open_paren) //exit
    {
        consume();
        consume();
        
        auto stmt_exit = m_allocator.alloc<NodeStmtExit>();
        if (auto node_expr = parse_expr())
        {
            stmt_exit->expr = node_expr.value();
        }
        else
        {
            error_expected("expression");
        }
        try_consume_err(TokenType::close_paren);
        try_consume_err(TokenType::semi);

        auto stmt = m_allocator.alloc<NodeStmt>();
        stmt->var = stmt_exit;
        return stmt;
    }
    if (peek().has_value() && peek().value().type == TokenType::let
             && peek(1).has_value() && peek(1).value().type == TokenType::ident
             && peek(2).has_value() && peek(2).value().type == TokenType::eq) //let
    {
        consume();
        auto stmt_let = m_allocator.alloc<NodeStmtLet>();
        stmt_let->ident = consume();
        consume();
        if (auto expr = parse_expr())
        {
            stmt_let->expr = expr.value();
        }
        else
        {
            error_expected("expression");
            //std::cerr << "Invalid variable expression" << std::endl;
        }
       
        try_consume_err(TokenType::semi);

        auto stmt = m_allocator.alloc<NodeStmt>();
        stmt->var = stmt_let;
        
        return stmt;
    }
    if (peek().has_value() && peek().value().type == TokenType::ident && peek(1).has_value() && peek(1).value().type == TokenType::eq)
    {
        auto assign = m_allocator.alloc<NodeStmtAsign>();
        assign->ident = consume();
        consume();
        if (auto expr = parse_expr())
        {
            assign->expr = expr.value();
        }
        else
        {
            error_expected("expression");
        }
        try_consume_err(TokenType::semi);
        
        auto stmt = m_allocator.alloc<NodeStmt>();
        stmt->var = assign;
        return stmt;
    }
    if (peek().has_value() && peek().value().type == TokenType::open_curly)
    {
        if (auto scope = parse_scope())
        {
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = scope.value();
            return stmt;
        }
        else
        {
            error_expected("scope");
        }
    }
    if (auto if_ = try_consume(TokenType::if_))
    {
        try_consume_err(TokenType::open_paren);
        auto stmt_if = m_allocator.alloc<NodeStmtIf>();
        if (auto expr = parse_expr())
        {
            stmt_if->expr = expr.value();
        }
        else
        {
            error_expected("expression");
        }
        try_consume_err(TokenType::close_paren);
        if (auto scope = parse_scope())
        {
            stmt_if->scope = scope.value();
        }
        else
        {
            error_expected("statement");
        }
        stmt_if->pred = parse_if_pred();
        
        auto stmt = m_allocator.alloc<NodeStmt>();
        stmt->var = stmt_if;
        return stmt;
    }
    return {};
}

std::optional<NodeProg> Parser::parse_prog()
{
    NodeProg prog;
    while (peek().has_value())
    {
        if (auto stmt = parse_stmt())
        {
            prog.stmts.push_back(stmt.value());
        }
        else
        {
            error_expected("statement");
        }
    }
    m_index = 0;
    return prog;
}

std::optional<Token> Parser::peek(int offset) const
{
    if (m_index + offset >= m_tokens.size())
    {
        return {};
    }
    else
    {
        return m_tokens.at(m_index + offset);
    }
}

Token Parser::consume()
{
    return m_tokens.at(m_index++);
}

Token Parser::try_consume_err(TokenType type)
{
    if (peek().has_value() && peek().value().type == type)
    {
        return consume();
    }
    else
    {
        error_expected(to_string(type));
        return {};
    }
}

std::optional<Token> Parser::try_consume(TokenType type)
{
    if (peek().has_value() && peek().value().type == type)
    {
        return consume();
    }
    else
    {
        return {};
    }
}

const void Parser::error_expected(const std::string& msg)
{
    std::cerr << "[Parser error] Expected " << msg << " on line " << peek(-1).value().line << std::endl;
    exit(1);
}
