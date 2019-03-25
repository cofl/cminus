#include "VariableASTNode.hpp"
#include "../asm/ASM.hpp"
#include <iostream>

namespace Cminus { namespace AST
{
    using std::endl;

    VariableASTNode::VariableASTNode(std::string id)
        :   ExpressionASTNode(ASTNodeType::Variable),
            ID(id),
            ArrayIndex(nullptr)
    {
        IsLValue = true;
        // TODO: type
    }

    VariableASTNode::VariableASTNode(std::string id, ExpressionASTNode* arrayIndex)
        :   ExpressionASTNode(ASTNodeType::Variable),
            ID(id),
            ArrayIndex(arrayIndex)
    {
        IsLValue = true;
        // TODO: type
    }

    ASTNode* VariableASTNode::Check(State& state)
    {
        auto data = Symbols->FindVariable(this->ID);
        this->Type = data->Type;
        if(nullptr != this->ArrayIndex)
        {
            ArrayIndex->Symbols = this->Symbols;
            ArrayIndex = (ExpressionASTNode*) ArrayIndex->Check(state);
        }
        return this;
    }

    void VariableASTNode::Emit(State& state, Register& destination)
    {
        auto data = Symbols->FindVariable(ID);
        if(data->IsGlobal)
        {
            if(nullptr != ArrayIndex)
            {
                state.SaveRegisters(1, RegisterIndex::RAX);
                auto eax = state.GetRegister(RegisterIndex::EAX, RegisterLength::_32);
                ArrayIndex->Emit(state, eax);
                auto reg = state.AllocRegister(RegisterLength::_64);
                state.OutputStream << "\tcdqe" << endl
                                   << "\tlea " << reg.Name() << ", 0[0+rax*4]"                         << endl
                                   << "\tlea rax, " << data->GlobalLocation << "[rip]"                 << endl
                                   << "\tmov " << destination.Name() << ", [rax+" << reg.Name() << ']' << endl;
                state.FreeRegister(reg);
                state.RestoreRegisters();
            } else
            {
                ASM::Load(state, destination, data->GlobalLocation);
            }
        } else if (nullptr != ArrayIndex)
        {
            state.SaveRegisters(1, RegisterIndex::EAX);
            auto eax = state.GetRegister(RegisterIndex::EAX, RegisterLength::_32);
            ArrayIndex->Emit(state, eax);
            auto reg = state.AllocRegister(RegisterLength::_64);
            state.OutputStream << "\tcdqe" << endl
                               << "\tlea " << reg.Name() << ", 0[0+rax*4]"                         << endl
                               << "\tlea rax, " << data->StackOffset << "[rbp]"                    << endl
                               << "\tmov " << destination.Name() << ", [rax+" << reg.Name() << ']' << endl;
            state.FreeRegister(reg);
            state.RestoreRegisters();
        } else
        {
            ASM::Load(state, destination, data->StackOffset);
        }
    }

    void VariableASTNode::EmitLValue(State& state, Register& destination)
    {
        auto data = Symbols->FindVariable(ID);
        if(data->IsGlobal)
        {
            if(nullptr != ArrayIndex)
            {
                auto savedAx = destination.Index != RegisterIndex::RAX;
                if (savedAx)
                    state.OutputStream << "\tpush rax" << endl;
                auto eax = state.GetRegister(RegisterIndex::EAX, RegisterLength::_32);
                ArrayIndex->Emit(state, eax);
                auto reg = state.AllocRegister(RegisterLength::_64);
                state.OutputStream << "\tcdqe" << endl
                                   << "\tlea " << reg.Name() << ", 0[0+rax*4]"                         << endl
                                   << "\tlea rax, " << data->GlobalLocation << "[rip]"                 << endl
                                   << "\tlea " << destination.Name() << ", [rax+" << reg.Name() << ']' << endl;
                state.FreeRegister(reg);
                if (savedAx)
                    state.OutputStream << "\tpop rax" << endl;
            } else
            {
                state.OutputStream << "\tlea " << destination.Name() << ", " << data->GlobalLocation << "[rip]" << endl;
            }
        } else if (nullptr != ArrayIndex)
        {
            auto savedAx = destination.Index != RegisterIndex::RAX;
            if (savedAx)
                state.OutputStream << "\tpush rax" << endl;
            auto eax = state.GetRegister(RegisterIndex::EAX, RegisterLength::_32);
            ArrayIndex->Emit(state, eax);
            auto reg = state.AllocRegister(RegisterLength::_64);
            state.OutputStream << "\tcdqe" << endl
                               << "\tlea " << reg.Name() << ", 0[0+rax*4]"                         << endl
                               << "\tlea rax, " << data->StackOffset << "[rbp]"                    << endl
                               << "\tlea " << destination.Name() << ", [rax+" << reg.Name() << ']' << endl;
            state.FreeRegister(reg);
            if (savedAx)
                state.OutputStream << "\tpop rax" << endl;
        } else
        {
            state.OutputStream << "\tlea " << destination.Name() << ", " << data->StackOffset << "[rbp]" << endl;
        }
    }
}}
