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

    ASTNode* ReadCallASTNode::Check(DriverState& state)
    {
        Variable->Symbols = this->Symbols;
        Variable = (ExpressionASTNode*) Variable->Check(state);
        return this;
    }

    ASTNode* WriteCallASTNode::Check(DriverState& state)
    {
        Value->Symbols = this->Symbols;
        Value = (ExpressionASTNode*) Value->Check(state);
        return this;
    }

    void ReadCallASTNode::Emit(DriverState& state)
    {
        auto rsii = state.GetRegisterID("rsi");
        state.FreeRegisters[rsii] = false; // manually reserve register
        Variable->EmitLValue(state, "rsi");
        state.OutputStream << "\tlea rdi, .int_rformat[rip]" << endl
                           << "\tmov eax, 0"                 << endl
                           << "\tcall scanf@PLT"             << endl;
        state.FreeRegisters[rsii] = true; // manually free register
    }

    void WriteCallASTNode::Emit(DriverState& state)
    {
        if(Value->Type == state.GetTypeID("string"))
        {
            // emit string write
            auto rdii = state.GetRegisterID("rdi");
            state.FreeRegisters[rdii] = false; // manually reserve register
            Value->Emit(state, "rdi");
            state.OutputStream << "\tcall puts@PLT" << endl; // mildly more efficient than printf
            state.FreeRegisters[rdii] = true; // manually free register
        } else
        {
            // emit value generation, then value write
            auto esii = state.GetRegisterID("edi");
            state.FreeRegisters[esii] = false; // manually reserve register
            Value->Emit(state, "esi");
            state.OutputStream << "\tlea rdi, .int_wformat[rip]" << endl
                               << "\tmov eax, 0"                 << endl
                               << "\tcall printf@PLT"            << endl;
            state.FreeRegisters[esii] = true; // manually free register
        }
    }
}}
