#include "ForStatementASTNode.hpp"
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
            auto eax = state.GetRegister(RegisterIndex::EAX, RegisterLength::_32);
            Test->Emit(state, eax);
            state.OutputStream << "\tcmp eax, 0"               << endl
                               << "\tje " << afterLabel << 'f' << endl
                               << beginLabel << ':'            << endl;
        } else
        {
            state.OutputStream << beginLabel << ':'            << endl;
        }

        Body->Emit(state);
        state.OutputStream << stepLabel << ':' << endl;
        if(nullptr != Step)
            Step->Emit(state);
        if(nullptr != Test)
        {
            auto eax = state.GetRegister(RegisterIndex::EAX, RegisterLength::_32);
            Test->Emit(state, eax);
            state.OutputStream << "\tcmp eax, 0"                << endl
                               << "\tjnz " << beginLabel << 'b' << endl
                               << afterLabel << ':'             << endl;
        } else
        {
            state.OutputStream << "\tjmp " << beginLabel << 'b' << endl
                               << afterLabel << ':'             << endl;
        }

        state.BreakLabels.pop_back();
        state.ContinueLabels.pop_back();
    }
}}
