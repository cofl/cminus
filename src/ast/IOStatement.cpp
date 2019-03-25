#include "IOStatement.hpp"
#include "../asm/ASM.hpp"
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
        ASM::LoadGlobalAddress(state, state.GetRegister(RegisterIndex::RDI, RegisterLength::_64), ".int_rformat");
        ASM::Zero(state, state.GetRegister(RegisterIndex::EAX, RegisterLength::_32));
        ASM::Call(state, "scanf@PLT");
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
            ASM::Call(state, "puts@PLT"); // mildly more efficient than printf
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
            ASM::LoadGlobalAddress(state, state.GetRegister(RegisterIndex::RDI, RegisterLength::_64), ".int_wformat");
            ASM::Zero(state, state.GetRegister(RegisterIndex::EAX, RegisterLength::_32));
            ASM::Call(state, "printf@PLT");
            state.FreeRegister(esi);
        }
    }
}}
