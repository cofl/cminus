#include "IfStatementASTNode.hpp"
#include "../asm/ASM.hpp"
#include <iostream>

namespace Cminus { namespace AST
{
    using std::endl;

    IfStatementASTNode::IfStatementASTNode(ExpressionASTNode* test, ASTNode* ifTrue)
        :   ASTNode(ASTNodeType::If),
            Test(test),
            IfTrue(ifTrue),
            IfFalse(nullptr)
    {
        // nop
    }

    IfStatementASTNode::IfStatementASTNode(ExpressionASTNode* test, ASTNode* ifTrue, ASTNode* ifFalse)
        :   ASTNode(ASTNodeType::If),
            Test(test),
            IfTrue(ifTrue),
            IfFalse(ifFalse)
    {
        // nop
    }

    ASTNode* IfStatementASTNode::Check(State& state)
    {
        Test->Symbols = this->Symbols;
        IfTrue->Symbols = this->Symbols;
        Test = (ExpressionASTNode*) Test->Check(state);
        IfTrue = IfTrue->Check(state);
        if(nullptr != IfFalse)
        {
            IfFalse->Symbols = this->Symbols;
            IfFalse = IfFalse->Check(state);
        }
        return this;
    }

    void IfStatementASTNode::Emit(State& state)
    {
        int elseLabel = state.NextFreeLabel;
        state.NextFreeLabel += 1;
        Register eax;
        if(!state.AllocRegister(RegisterIndex::EAX, RegisterLength::_32, eax))
            throw "Register EAX already allocated!";
        Test->Emit(state, eax);
        state.FreeRegister(eax);
        ASM::CmpAndJump(state, "je", elseLabel, 'f', eax);
        IfTrue->Emit(state);
        if(IfFalse != nullptr)
        {
            int afterLabel = state.NextFreeLabel;
            state.NextFreeLabel += 1;
            ASM::JumpForward(state, afterLabel);
            ASM::Label(state, elseLabel);
            IfFalse->Emit(state);
            ASM::Label(state, afterLabel);
            state.NextFreeLabel -= 2;
        } else
        {
            ASM::Label(state, elseLabel);
            state.NextFreeLabel -= 1;
        }
    }
}}
