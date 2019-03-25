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
        Register reg, rdi, eax;
        if(!state.AllocRegister(RegisterIndex::RSI, RegisterLength::_64, reg))
            throw "Register RSI already allocated!";
        if(!state.AllocRegister(RegisterIndex::RDI, RegisterLength::_64, rdi))
            throw "Register RDI already allocated!";
        if(!state.AllocRegister(RegisterIndex::EAX, RegisterLength::_32, eax))
            throw "Register EAX already allocated!";
        Variable->EmitLValue(state, reg);
        ASM::LoadGlobalAddress(state, rdi, ".int_rformat");
        ASM::Zero(state, state.GetRegister(RegisterIndex::EAX, RegisterLength::_32));
        ASM::Call(state, "scanf@PLT");
        state.FreeRegister(reg);
        state.FreeRegister(rdi);
        state.FreeRegister(eax);
    }

    void WriteCallASTNode::Emit(State& state)
    {
        if(Value->Type == state.GetTypeID("string"))
        {
            // emit string write
            Register rdi;
            if(!state.AllocRegister(RegisterIndex::RDI, RegisterLength::_64, rdi))
                throw "Register RDI already allocated!";
            Value->Emit(state, rdi);
            ASM::Call(state, "puts@PLT"); // mildly more efficient than printf
            state.FreeRegister(rdi);
        } else
        {
            // emit value generation, then value write
            Register esi, rdi, eax;
            if(!state.AllocRegister(RegisterIndex::ESI, RegisterLength::_32, esi))
                throw "Register ESI already allocated!";
            if(!state.AllocRegister(RegisterIndex::RDI, RegisterLength::_64, rdi))
                throw "Register RDI already allocated!";
            if(!state.AllocRegister(RegisterIndex::EAX, RegisterLength::_32, eax))
                throw "Register EAX already allocated!";
            Value->Emit(state, esi);
            ASM::LoadGlobalAddress(state, rdi, ".int_wformat");
            ASM::Zero(state, eax);
            ASM::Call(state, "printf@PLT");
            state.FreeRegister(esi);
            state.FreeRegister(rdi);
            state.FreeRegister(eax);
        }
    }
}}
