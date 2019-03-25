#include "IfStatementASTNode.hpp"
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
        auto eax = state.GetRegister(RegisterIndex::EAX, RegisterLength::_32);
        Test->Emit(state, eax);
        state.OutputStream << "\tcmp eax, 0"              << endl
                           << "\tje " << elseLabel << 'f' << endl;
        IfTrue->Emit(state);
        if(IfFalse != nullptr)
        {
            int afterLabel = state.NextFreeLabel;
            state.NextFreeLabel += 1;
            state.OutputStream << "\tjmp " << afterLabel << 'f' << endl
                               << elseLabel << ':'              << endl;
            IfFalse->Emit(state);
            state.OutputStream << afterLabel << ':' << endl;
            state.NextFreeLabel -= 2;
        } else
        {
            state.OutputStream << elseLabel << ':' << endl;
            state.NextFreeLabel -= 1;
        }
    }
}}
