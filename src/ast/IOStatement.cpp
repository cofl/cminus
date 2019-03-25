#include "IOStatement.hpp"
#include <iostream>

namespace Cminus { namespace AST
{
    using std::endl;

    ReadCallASTNode::ReadCallASTNode(ExpressionASTNode* variable)
        :   ASTNode(ASTNodeType::ReadCall),
            Variable(variable)
    {
        // nop
    }

    WriteCallASTNode::WriteCallASTNode(ExpressionASTNode* value)
        :   ASTNode(ASTNodeType::WriteCall),
            Value(value)
    {
        // nop
    }

    ASTNode* ReadCallASTNode::Check(State& state)
    {
        Variable->Symbols = this->Symbols;
        Variable = (ExpressionASTNode*) Variable->Check(state);
        return this;
    }

    ASTNode* WriteCallASTNode::Check(State& state)
    {
        Value->Symbols = this->Symbols;
        Value = (ExpressionASTNode*) Value->Check(state);
        return this;
    }

    void ReadCallASTNode::Emit(State& state)
    {
        Register reg;
        if(!state.AllocRegister(RegisterIndex::RSI, RegisterLength::_64, reg))
        {
            throw "Register RSI already in use!";
        }
        Variable->EmitLValue(state, reg);
        state.OutputStream << "\tlea rdi, .int_rformat[rip]" << endl
                           << "\tmov eax, 0"                 << endl
                           << "\tcall scanf@PLT"             << endl;
        state.FreeRegister(reg);
    }

    void WriteCallASTNode::Emit(State& state)
    {
        if(Value->Type == state.GetTypeID("string"))
        {
            // emit string write
            Register rdi;
            if(!state.AllocRegister(RegisterIndex::RDI, RegisterLength::_64, rdi))
            {
                throw "Register RDI already in use!";
            }
            Value->Emit(state, rdi);
            state.OutputStream << "\tcall puts@PLT" << endl; // mildly more efficient than printf
            state.FreeRegister(rdi);
        } else
        {
            // emit value generation, then value write
            Register esi;
            if(!state.AllocRegister(RegisterIndex::ESI, RegisterLength::_32, esi))
            {
                throw "Register ESI already in use!";
            }
            Value->Emit(state, esi);
            state.OutputStream << "\tlea rdi, .int_wformat[rip]" << endl
                               << "\tmov eax, 0"                 << endl
                               << "\tcall printf@PLT"            << endl;
            state.FreeRegister(esi);
        }
    }
}}
