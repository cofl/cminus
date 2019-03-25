#include "FunctionCallASTNode.hpp"
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
        state.SaveRegisters(6, RegisterIndex::RDI, RegisterIndex::RSI, RegisterIndex::RDX,
            RegisterIndex::RCX, RegisterIndex::R9, RegisterIndex::R9);
        int argc = Arguments.size();
        if(argc > 0)
        {
            auto edi = state.GetRegister(RegisterIndex::EDI, RegisterLength::_32);
            Arguments[0]->Emit(state, edi);
        }
        if (argc > 1)
        {
            auto esi = state.GetRegister(RegisterIndex::ESI, RegisterLength::_32);
            Arguments[1]->Emit(state, esi);
        }
        if (argc > 2)
        {
            auto edx = state.GetRegister(RegisterIndex::EDX, RegisterLength::_32);
            Arguments[2]->Emit(state, edx);
        }
        if (argc > 3)
        {
            auto ecx = state.GetRegister(RegisterIndex::ECX, RegisterLength::_32);
            Arguments[3]->Emit(state, ecx);
        }
        if (argc > 4)
        {
            auto r8d = state.GetRegister(RegisterIndex::R8D, RegisterLength::_32);
            Arguments[4]->Emit(state, r8d);
        }
        if (argc > 5)
        {
            auto r9d = state.GetRegister(RegisterIndex::R9D, RegisterLength::_32);
            Arguments[5]->Emit(state, r9d);
        }
        if (argc > 6)
        {
            int t = 4 * (argc - 6);
            t += 16 - (t % 16);
            state.OutputStream << "\tsub rsp," << t << endl;
            auto reg = state.AllocRegister(RegisterLength::_32);
            for(int i = 6, j = 0; i < argc; i += 1, j += 1)
            {
                Arguments[i]->Emit(state, reg);
                state.OutputStream << "\tmov " << j << "[rsp]," << reg.Name() << endl;
            }
            state.FreeRegister(reg);
        }
        state.OutputStream << "\tmov eax, 0"  << endl
                           << "\tcall " << ID << endl;
        if(destination.Index != RegisterIndex::EAX)
            state.OutputStream << "\tmov " << destination.Name() << ", eax" << endl;
        state.RestoreRegisters();
    }
}}
