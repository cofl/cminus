#include "VariableASTNode.hpp"
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

    ASTNode* VariableASTNode::Check(DriverState& state)
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

    void VariableASTNode::Emit(DriverState& state, const char* destinationRegister)
    {
        auto data = Symbols->FindVariable(ID);
        if(data->IsGlobal)
        {
            if(nullptr != ArrayIndex)
            {
                auto savedAx = state.GetRegisterID(destinationRegister) != state.GetRegisterID("eax");
                if (savedAx)
                    state.OutputStream << "\tpush rax" << endl;
                ArrayIndex->Emit(state, "eax");
                auto tri = state.GetFreeRegister();
                auto trn64 = state.RegisterNames64[tri];
                state.OutputStream << "\tcdqe" << endl
                                   << "\tlea " << trn64 << ", 0[0+rax*4]"              << endl
                                   << "\tlea rax, " << data->GlobalLocation << "[rip]" << endl
                                   << "\tmov " << destinationRegister << ", [rax+" << trn64 << ']' << endl;
                state.ReleaseRegister(tri);
                if (savedAx)
                    state.OutputStream << "\tpop rax" << endl;
            } else
            {
                state.OutputStream << "\tmov " << destinationRegister << ", " << data->GlobalLocation << "[rip]" << endl;
            }
        } else if (nullptr != ArrayIndex)
        {
            auto savedAx = state.GetRegisterID(destinationRegister) != state.GetRegisterID("eax");
            if (savedAx)
                state.OutputStream << "\tpush rax" << endl;
            ArrayIndex->Emit(state, "eax");
            auto tri = state.GetFreeRegister();
            auto trn64 = state.RegisterNames64[tri];
            state.OutputStream << "\tcdqe" << endl
                               << "\tlea " << trn64 << ", 0[0+rax*4]"           << endl
                               << "\tlea rax, " << data->StackOffset << "[rbp]" << endl
                               << "\tmov " << destinationRegister << ", [rax+" << trn64 << ']' << endl;
            state.ReleaseRegister(tri);
            if (savedAx)
                state.OutputStream << "\tpop rax" << endl;
        } else
        {
            state.OutputStream << "\tmov " << destinationRegister << ", " << data->StackOffset << "[rbp]" << endl;
        }
    }

    void VariableASTNode::EmitLValue(DriverState& state, const char* destinationRegister)
    {
        auto data = Symbols->FindVariable(ID);
        if(data->IsGlobal)
        {
            if(nullptr != ArrayIndex)
            {
                auto savedAx = state.GetRegisterID(destinationRegister) != state.GetRegisterID("eax");
                if (savedAx)
                    state.OutputStream << "\tpush rax" << endl;
                ArrayIndex->Emit(state, "eax");
                auto tri = state.GetFreeRegister();
                auto trn64 = state.RegisterNames64[tri];
                state.OutputStream << "\tcdqe" << endl
                                   << "\tlea " << trn64 << ", 0[0+rax*4]"              << endl
                                   << "\tlea rax, " << data->GlobalLocation << "[rip]" << endl
                                   << "\tlea " << destinationRegister << ", [rax+" << trn64 << ']' << endl;
                state.ReleaseRegister(tri);
                if (savedAx)
                    state.OutputStream << "\tpop rax" << endl;
            } else
            {
                state.OutputStream << "\tlea " << destinationRegister << ", " << data->GlobalLocation << "[rip]" << endl;
            }
        } else if (nullptr != ArrayIndex)
        {
            auto savedAx = state.GetRegisterID(destinationRegister) != state.GetRegisterID("eax");
            if (savedAx)
                state.OutputStream << "\tpush rax" << endl;
            ArrayIndex->Emit(state, "eax");
            auto tri = state.GetFreeRegister();
            auto trn64 = state.RegisterNames64[tri];
            state.OutputStream << "\tcdqe" << endl
                               << "\tlea " << trn64 << ", 0[0+rax*4]"           << endl
                               << "\tlea rax, " << data->StackOffset << "[rbp]" << endl
                               << "\tlea " << destinationRegister << ", [rax+" << trn64 << ']' << endl;
            state.ReleaseRegister(tri);
            if (savedAx)
                state.OutputStream << "\tpop rax" << endl;
        } else
        {
            state.OutputStream << "\tlea " << destinationRegister << ", " << data->StackOffset << "[rbp]" << endl;
        }
    }
}}
