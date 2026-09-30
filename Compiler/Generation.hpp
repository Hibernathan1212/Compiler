//
//  Generation.hpp
//  Compiler
//
//  Created by Nathan Thurber on 30/6/24.
//

#include "Parser.hpp"

#pragma once

class Generator
{
public:
    Generator(NodeProg prog);
    
    void gen_bin_expr(const NodeBinExpr* bin_expr);
    void gen_term(const NodeTerm* term);
    void gen_expr(const NodeExpr* expr);
    void gen_scope(const NodeScope* scope);
    void gen_if_pred(const NodeIfPred* pred, const std::string& end_label);
    void gen_stmt(const NodeStmt* stmt);
    std::string gen_prog();
private:
    
    void push(const std::string& reg);
    void pop(const std::string& reg);
    
    void begin_scope();
    void end_scope();
    
    std::string create_label();
    
    struct Var
    {
        std::string name;
        size_t stack_loc;
    };
    
    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    std::vector<Var> m_vars {};
    std::vector<size_t> m_scopes {};
    int m_label_count = 0;
};
