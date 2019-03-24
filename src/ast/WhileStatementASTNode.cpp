#include "WhileStatementASTNode.hpp"
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

    ASTNode* WhileStatementASTNode::Check(DriverState& state)
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

    void WhileStatementASTNode::Emit(DriverState& state)
    {
        int beginLabel = state.TemporaryLabelIndex;
        int testLabel = state.TemporaryLabelIndex + 1;
        int afterLabel = state.TemporaryLabelIndex + 2;
        state.TemporaryLabelIndex += 3;
        state.ContinueLabels.push_back(testLabel);
        state.BreakLabels.push_back(afterLabel);
        state.OutputStream << "\tjmp " << testLabel << 'f'  << endl
                           << beginLabel << ':'             << endl;
        Body->Emit(state);
        state.OutputStream << testLabel << ':'              << endl;
        Test->Emit(state, "eax");
        state.OutputStream << "\tcmp eax, 0"                << endl
                           << "\tjnz " << beginLabel << 'b' << endl
                           << afterLabel << ':'             << endl;
        state.TemporaryLabelIndex -= 2;
        state.BreakLabels.pop_back();
        state.ContinueLabels.pop_back();
    }
}}
