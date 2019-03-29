#include "StatementListASTNode.hpp"
#include "../asm/ASM.hpp"
#include <iostream>
#include <sys/utsname.h>

namespace Cminus { namespace AST
{
    using namespace Cminus::Structures;
    using std::endl;

    StatementListASTNode::StatementListASTNode()
        :   ASTNode(ASTNodeType::StatementList)
    {
        // nop
    }

    ASTNode* StatementListASTNode::Check(State& state)
    {
        if(nullptr == Symbols)
        {
            Symbols = state.SymbolStack.back();
        } else
        {
            Symbols = new SymbolTable(state.SymbolStack.back());
            state.SymbolStack.push_back(Symbols);
        }
        int len = Members.size();
        for(int i = 0; i < len; i += 1)
        {
            auto member = Members[i];
            member->Symbols = Symbols;
            Members[i] = member->Check(state);
        }
        state.SymbolStack.pop_back(); // get our scope off the stack
        return this;
    }

    void StatementListASTNode::Emit(State& state)
    {
        int size = Symbols->GetByteSize();
        if(size > 0)
        {
            int base = state.GetStackOffset();
            ASM::IncreaseStack(state, size);
            for(auto it = Symbols->Variables.begin(); it != Symbols->Variables.end(); it++)
            {
                auto data = it->second;
                data->IsGlobal = false;
                data->StackOffset = data->ArraySize * -4 - base;
                base -= data->StackOffset;
                if(nullptr != data->InitialValue)
                {
                    // TODO: emit initializer code
                }
            }
        }
        for(auto const& member: Members)
            member->Emit(state);
        if(size > 0)
            ASM::DecreaseStack(state, size);
    }
}}
