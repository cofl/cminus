#include "WhileStatementASTNode.hpp"
#include "../asm/ASM.hpp"
#include <iostream>

namespace Cminus { namespace AST
{
    using std::endl;

    WhileStatementASTNode::WhileStatementASTNode(ExpressionASTNode* test, ASTNode* body)
        :   ASTNode(ASTNodeType::While),
            Test(test),
            Body(body)
    {

    }

    ASTNode* WhileStatementASTNode::Check(State& state)
    {
        Test->Symbols = this->Symbols;
        Body->Symbols = this->Symbols;
        Test = (ExpressionASTNode*) Test->Check(state);
        state.BreakLabels.push_back(0);
        state.ContinueLabels.push_back(0);
        Body = (ExpressionASTNode*) Body->Check(state);
        state.BreakLabels.pop_back();
        state.ContinueLabels.pop_back();
        return this;
    }

    void WhileStatementASTNode::Emit(State& state)
    {
        int beginLabel = state.NextFreeLabel;
        int testLabel = state.NextFreeLabel + 1;
        int afterLabel = state.NextFreeLabel + 2;
        state.NextFreeLabel += 3;
        state.ContinueLabels.push_back(testLabel);
        state.BreakLabels.push_back(afterLabel);
        ASM::JumpForward(state, testLabel);
        ASM::Label(state, beginLabel);
        Body->Emit(state);
        ASM::Label(state, testLabel);
        Test->Emit(state, state.GetRegister(RegisterIndex::EAX, RegisterLength::_32));
        ASM::CmpAndJump(state, "jnz", beginLabel, 'b', state.GetRegister(RegisterIndex::EAX, RegisterLength::_32));
        ASM::Label(state, afterLabel);
        state.NextFreeLabel -= 3;
        state.BreakLabels.pop_back();
        state.ContinueLabels.pop_back();
    }
}}
