//
//  Generation.cpp
//  Compiler
//
//  Created by Nathan Thurber on 30/6/24.
//

#include "Generation.hpp"

Generator::Generator(NodeProg prog)
    : m_prog(std::move(prog)) {}

void Generator::gen_term(const NodeTerm* term)
{
    struct TermVisitor
    {
        Generator& gen;
        void operator()(const NodeTermIntLit* term_int_lit)
        {
            gen.m_output << "    mov rax, " << term_int_lit->int_lit.value.value() << "\n";
            gen.push("rax");
        }
        void operator()(const NodeTermIdent* term_ident)
        {
            auto it = std::find_if(gen.m_vars.cbegin(), gen.m_vars.cend(), [&](const Var& var) {return var.name == term_ident->ident.value.value();});
            if (it == gen.m_vars.cend())
            {
                std::cerr << "Undeclared identifier: " << term_ident->ident.value.value() << std::endl;
            }
            
            std::stringstream offset;
            offset << "QWORD [rsp + " << (gen.m_stack_size - it->stack_loc - 1) * 8 << "]";
            gen.push(offset.str());
        }
        void operator()(const NodeTermParen* term_paren)
        {
            gen.gen_expr(term_paren->expr);
        }
    };
    
    TermVisitor visitor { .gen = *this} ;
    std::visit(visitor, term->var);
}

void Generator::gen_bin_expr(const NodeBinExpr* bin_expr)
{
    struct BinExprVisitor
    {
        Generator& gen;
        void operator()(const NodeBinExprAdd* add)
        {
            gen.gen_expr(add->rhs);
            gen.gen_expr(add->lhs);
            gen.pop("rax");
            gen.pop("rbx");
            gen.m_output << "    add rax, rbx\n";
            gen.push("rax");
        }
        void operator()(const NodeBinExprSub* minus)
        {
            gen.gen_expr(minus->rhs);
            gen.gen_expr(minus->lhs);
            gen.pop("rax");
            gen.pop("rbx");
            gen.m_output << "    sub rax, rbx\n";
            gen.push("rax");
        }
        void operator()(const NodeBinExprMulti* multi)
        {
            gen.gen_expr(multi->rhs);
            gen.gen_expr(multi->lhs);
            gen.pop("rax");
            gen.pop("rbx");
            gen.m_output << "    mul rbx\n";
            gen.push("rax");
        }
        void operator()(const NodeBinExprDiv* fslash)
        {
            gen.gen_expr(fslash->rhs);
            gen.gen_expr(fslash->lhs);
            gen.pop("rax");
            gen.pop("rbx");
            gen.m_output << "    div rbx\n";
            gen.push("rax");
        }
    };
    
    BinExprVisitor visitor { .gen = *this} ;
    std::visit(visitor, bin_expr->var);
}

void Generator::gen_expr(const NodeExpr* expr)
{
    struct ExprVisitor
    {
        Generator& gen;
        void operator()(const NodeTerm* term)
        {
            gen.gen_term(term);
        }
        void operator()(const NodeBinExpr* bin_expr)
        {
            gen.gen_bin_expr(bin_expr);
        }
        
    };
    
    ExprVisitor visitor { .gen = *this} ;
    std::visit(visitor, expr->var);
}

void Generator::gen_scope(const NodeScope* scope)
{
    begin_scope();
    for (const NodeStmt* stmt : scope->stmts)
    {
        gen_stmt(stmt);
    }
    end_scope();
}

void Generator::gen_if_pred(const NodeIfPred* pred, const std::string& end_label)
{
    struct PredVisitor
    {
        Generator& gen;
        const std::string& end_label;
        void operator()(const NodeIfPredElif* elif)
        {
            gen.gen_expr(elif->expr);
            gen.pop("rax");
            std::string label = gen.create_label();
            gen.m_output << "    test rax, rax\n";
            gen.m_output << "    jz " << label << "\n";
            gen.gen_scope(elif->scope);
            gen.m_output << "    jmp " << end_label << "\n";
            if (elif->pred.has_value())
            {
                gen.m_output << label << ":\n";
                gen.gen_if_pred(elif->pred.value(), end_label);
            }
        }
        
        void operator()(const NodeIfPredElse* else_)
        {
            gen.gen_scope(else_->scope);
        }
    };
    
    PredVisitor visitor { .gen = *this, .end_label = end_label };
    std::visit(visitor, pred->var);
}

void Generator::gen_stmt(const NodeStmt* stmt)
{
    struct StmtVisitor
    {
        Generator& gen;
        void operator()(const NodeStmtExit* stmt_exit)
        {
            gen.gen_expr(stmt_exit->expr);
            gen.m_output  << "    mov rax, 60\n";
            gen.pop("rdi");
            gen.m_output << "    syscall\n";
        }
        
        void operator()(const NodeStmtLet* stmt_let)
        {
            auto it = std::find_if(gen.m_vars.cbegin(), gen.m_vars.cend(), [&](const Var& var) {return var.name == stmt_let->ident.value.value();});
            if (it != gen.m_vars.cend())
            {
                std::cerr << "Identifier already used: " << stmt_let->ident.value.value() << std::endl;
            }
            gen.m_vars.push_back( {.name = stmt_let->ident.value.value(),  .stack_loc = gen.m_stack_size } );
            gen.gen_expr(stmt_let->expr);
        }
        
        void operator()(const NodeScope* scope)
        {
            gen.gen_scope(scope);
        }
        
        void operator()(const NodeStmtIf* stmt_if)
        {
            gen.gen_expr(stmt_if->expr);
            gen.pop("rax");
            std::string label = gen.create_label();
            gen.m_output << "    test rax, rax\n";
            gen.m_output << "    jz " << label << "\n";
            gen.gen_scope(stmt_if->scope);
            gen. m_output << "    jmp " << label << "\n";
            if (stmt_if->pred.has_value())
            {
                std::string end_label = gen.create_label();
                gen.m_output << "    jmp " << end_label << "\n";
                gen.gen_if_pred(stmt_if->pred.value(), end_label);
                gen.m_output << end_label << ": \n";
            }
            else
            {
                gen.m_output << label << ";\n";
            }
        }
        void operator()(const NodeStmtAsign* stmt_asign)
        {
            auto it = std::find_if(gen.m_vars.cbegin(), gen.m_vars.cend(), [&](const Var& var) {return var.name == stmt_asign->ident.value.value();});
            if (it == gen.m_vars.end())
            {
                std::cerr << "Undeclared identififer: " << stmt_asign->ident.value.value() << std::endl;
            }
            gen.gen_expr(stmt_asign->expr);
            gen.pop("rax");
            gen.m_output << "    mov [rsp" << (gen.m_stack_size - it->stack_loc - 1) * 8 << "], rax\n";
        }
    };
    
    StmtVisitor visitor { .gen = *this };
    std::visit(visitor, stmt->var);
}

std::string Generator::gen_prog() //x86 linux
{
    m_output << "global _start\n_start:\n";
    
    for (const NodeStmt* stmt : m_prog.stmts)
    {
        gen_stmt(stmt);
    }
    
    m_output << "    mov rax, 60\n";
    m_output << "    mov rdi, 0\n";
    m_output << "    syscall";
    return m_output.str();
    
}

void Generator::push(const std::string& reg)
{
    m_output << "    push " << reg << "\n";
    m_stack_size++;
}

void Generator::pop(const std::string& reg)
{
    m_output << "    pop " << reg << "\n";
    m_stack_size--;
}

void Generator::begin_scope()
{
    m_scopes.push_back(m_vars.size());
}

void Generator::end_scope()
{
    size_t pop_count = m_vars.size() - m_scopes.back();
    m_output << "    add rsp, " << pop_count * 8 << "\n";
    m_stack_size -= pop_count;
    for (int i = 0; i < pop_count; i++)
    {
        m_vars.pop_back();
    }
    m_scopes.pop_back();
}

std::string Generator::create_label()
{
    std::stringstream ss;
    ss << "label" << m_label_count++;
    return ss.str();
}
