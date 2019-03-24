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

    ASTNode* FunctionCallASTNode::Check(DriverState& state)
    {
        auto data = Symbols->FindFunction(this->ID);
        if(data == nullptr)
        {
            // if we haven't declared it, assume it's in the standard library.
            this->Type = state.GetTypeID("int");
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

    void FunctionCallASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        bool usingEax = state.GetRegisterID(destinationRegister) == state.GetRegisterID("eax");
        state.SaveRegisters(6, "rdi", "rsi", "rdx", "rcx", "r8", "r9");
        int argc = Arguments.size();
        if(argc > 0)
            Arguments[0]->Emit(state, "edi");
        if (argc > 1)
            Arguments[1]->Emit(state, "esi");
        if (argc > 2)
            Arguments[2]->Emit(state, "edx");
        if (argc > 3)
            Arguments[3]->Emit(state, "ecx");
        if (argc > 4)
            Arguments[4]->Emit(state, "r8d");
        if (argc > 5)
            Arguments[5]->Emit(state, "r9d");
        if (argc > 6)
        {
            int t = 4 * (argc - 6);
            t += 16 - (t % 16);
            state.OutputStream << "\tsub rsp," << t << endl;
            auto tri = state.GetFreeRegister();
            auto trn = state.RegisterNames32[tri];
            for(int i = 6, j = 0; i < argc; i += 1, j += 1)
            {
                Arguments[i]->Emit(state, trn);
                state.OutputStream << "\tmov " << j << "[rsp]," << trn << endl;
            }
            state.ReleaseRegister(tri);
        }
        state.OutputStream << "\tmov eax, 0"  << endl
                           << "\tcall " << ID << endl;
        if(!usingEax)
            state.OutputStream << "\tmov " << destinationRegister << ", eax" << endl;
        state.RestoreRegisters();
    }
}}
