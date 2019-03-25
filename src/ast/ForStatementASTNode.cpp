#include "ForStatementASTNode.hpp"
#include "../asm/ASM.hpp"
#include <iostream>

namespace Cminus { namespace AST
{
    using std::endl;

    ForStatementASTNode::ForStatementASTNode(ExpressionASTNode* initial, ExpressionASTNode* test, ExpressionASTNode* step, ASTNode* body)
        :   ASTNode(ASTNodeType::For),
            Initial(initial),
            Test(test),
            Step(step),
            Body(body)
    {
        // nop
    }

    ASTNode* ForStatementASTNode::Check(State& state)
    {
        if(nullptr != Initial)
        {
            Initial->Symbols = this->Symbols;
            Initial = (ExpressionASTNode*) Initial->Check(state);
        }
        if(nullptr != Step)
        {
            Step->Symbols = this->Symbols;
            Step = (ExpressionASTNode*) Step->Check(state);
        }
        if(nullptr != Test)
        {
            Test->Symbols = this->Symbols;
            Test = (ExpressionASTNode*) Test->Check(state);
        }
        state.BreakLabels.push_back(0);
        state.ContinueLabels.push_back(0);
        Body->Symbols = this->Symbols;
        Body = Body->Check(state);
        state.BreakLabels.pop_back();
        state.ContinueLabels.pop_back();
        return this;
    }

    void ForStatementASTNode::Emit(State& state)
    {
        int beginLabel = state.NextFreeLabel;
        int stepLabel = state.NextFreeLabel + 1;
        int afterLabel = state.NextFreeLabel + 2;
        state.NextFreeLabel += 3;
        state.ContinueLabels.push_back(stepLabel);
        state.BreakLabels.push_back(afterLabel);
        if(nullptr != Initial)
            Initial->Emit(state);
        if(nullptr != Test)
        {
            Register eax;
            if(!state.AllocRegister(RegisterIndex::EAX, RegisterLength::_32, eax))
                throw "Register EAX already allocated!";
            Test->Emit(state, eax);
            ASM::CmpAndJump(state, "je", afterLabel, 'f', eax);
            state.FreeRegister(eax);
        }
        ASM::Label(state, beginLabel);

        Body->Emit(state);
        ASM::Label(state, stepLabel);
        if(nullptr != Step)
            Step->Emit(state);
        if(nullptr != Test)
        {
            Register eax;
            if(!state.AllocRegister(RegisterIndex::EAX, RegisterLength::_32, eax))
                throw "Register EAX already allocated!";
            Test->Emit(state, eax);
            ASM::CmpAndJump(state, "jnz", beginLabel, 'b', eax);
            state.FreeRegister(eax);
        } else
        {
            ASM::JumpBackward(state, beginLabel);
        }
        ASM::Label(state, afterLabel);

        state.BreakLabels.pop_back();
        state.ContinueLabels.pop_back();
    }
}}
