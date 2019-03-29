#include "FunctionCallASTNode.hpp"
#include "../asm/ASM.hpp"
#include <iostream>

namespace Cminus { namespace AST
{
    using std::endl;

    FunctionCallASTNode::FunctionCallASTNode(std::string id)
        :   ExpressionASTNode(ASTNodeType::FunctionCall),
            ID(id),
            Arguments()
    {
        // nop
    }

    FunctionCallASTNode::FunctionCallASTNode(std::string id, ExpressionListASTNode* args)
        :   ExpressionASTNode(ASTNodeType::FunctionCall),
            ID(id)
    {
        Arguments = args->Members;
    }

    ASTNode* FunctionCallASTNode::Check(State& state)
    {
        auto data = Symbols->FindFunction(this->ID);
        if(data == nullptr)
        {
            // if we haven't declared it, assume it's in the standard library.
            this->Type = state.GetTypeID("int"); // TODO
            this->ID = this->ID.append("@PLT");
        } else
        {
            this->Type = data->ReturnType;
        }
        for(int i = 0; i < Arguments.size(); i += 1)
        {
            auto member = Arguments[i];
            member->Symbols = this->Symbols;
            Arguments[i] = (ExpressionASTNode*) member->Check(state);
            if(nullptr != data && member->Type != data->Arguments[i].TypeID)
                throw std::string("Nonmatching types on argument ").append(to_string(i)); // TODO: better message
        }
        return this;
    }

    void FunctionCallASTNode::Emit(State& state, Register& destination)
    {
        state.SaveRegisters(destination, 7, RegisterIndex::RAX,
            RegisterIndex::RDI, RegisterIndex::RSI, RegisterIndex::RDX,
            RegisterIndex::RCX, RegisterIndex::R9, RegisterIndex::R9);
        int argc = Arguments.size();
        vector<Register> used_registers;
        ASM::AlignStack(state);
        if(argc > 0)
        {
            Register edi;
            if(!state.AllocRegister(RegisterIndex::EDI, RegisterLength::_32, edi))
                throw "Register EDI already allocated!";
            used_registers.push_back(edi);
            Arguments[0]->Emit(state, edi);
        }
        if (argc > 1)
        {
            Register esi;
            if(!state.AllocRegister(RegisterIndex::ESI, RegisterLength::_32, esi))
                throw "Register ESI already allocated!";
            used_registers.push_back(esi);
            Arguments[1]->Emit(state, esi);
        }
        if (argc > 2)
        {
            Register edx;
            if(!state.AllocRegister(RegisterIndex::EDX, RegisterLength::_32, edx))
                throw "Register EDX already allocated!";
            used_registers.push_back(edx);
            Arguments[2]->Emit(state, edx);
        }
        if (argc > 3)
        {
            Register ecx;
            if(!state.AllocRegister(RegisterIndex::ECX, RegisterLength::_32, ecx))
                throw "Register ECX already allocated!";
            used_registers.push_back(ecx);
            Arguments[3]->Emit(state, ecx);
        }
        if (argc > 4)
        {
            Register r8d;
            if(!state.AllocRegister(RegisterIndex::R8D, RegisterLength::_32, r8d))
                throw "Register R8D already allocated!";
            used_registers.push_back(r8d);
            Arguments[4]->Emit(state, r8d);
        }
        if (argc > 5)
        {
            Register r9d;
            if(!state.AllocRegister(RegisterIndex::R9D, RegisterLength::_32, r9d))
                throw "Register R9D already allocated!";
            used_registers.push_back(r9d);
            Arguments[5]->Emit(state, r9d);
        }
        if (argc > 6)
        {
            int t = 4 * (argc - 6);
            t += 16 - (t % 16);
            ASM::IncreaseStack(state, t);
            auto reg = state.AllocRegister(RegisterLength::_32);
            for(int i = 6, j = 0; i < argc; i += 1, j += 1)
            {
                Arguments[i]->Emit(state, reg);
                ASM::Store(state, j, "rsp", reg);
            }
            state.FreeRegister(reg);
        }

        Register eax = destination;
        if (destination.Index != RegisterIndex::EAX)
        {
            if(!state.AllocRegister(RegisterIndex::EAX, RegisterLength::_32, eax))
                throw "Register EAX already allocated!";
            used_registers.push_back(eax);
        }
        ASM::Zero(state, eax);
        ASM::Call(state, ID);
        if (destination.Index != RegisterIndex::EAX)
            ASM::Move(state, destination, eax);
        ASM::UnalignStack(state);
        for (auto&& r: used_registers)
            state.FreeRegister(r);
        state.RestoreRegisters();
    }
}}
