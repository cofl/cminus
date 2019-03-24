#include "MiscStatement.hpp"
#include <iostream>

namespace Cminus { namespace AST
{
    using std::endl;

    BreakStatementASTNode::BreakStatementASTNode()
        :   ASTNode(ASTNodeType::Break)
    {
        // nop
    }

    ContinueStatementASTNode::ContinueStatementASTNode()
        :   ASTNode(ASTNodeType::Continue)
    {
        // nop
    }

    ExitStatementASTNode::ExitStatementASTNode()
        :   ASTNode(ASTNodeType::Exit)
    {
        // nop
    }

    NopStatementASTNode::NopStatementASTNode()
        :   ASTNode(ASTNodeType::Nop)
    {
        // nop
    }

    ReturnStatementASTNode::ReturnStatementASTNode(ExpressionASTNode* value)
        :   ASTNode(ASTNodeType::Return),
            Value(value)
    {
        // nop
    }

    ASTNode* BreakStatementASTNode::Check(DriverState& state)
    {
        if(state.BreakLabels.size() == 0)
            throw "Illegal break statement.";
        return this;
    }

    ASTNode* ContinueStatementASTNode::Check(DriverState& state)
    {
        if(state.ContinueLabels.size() == 0)
            throw "Illegal continue statement.";
        return this;
    }

    ASTNode* ReturnStatementASTNode::Check(DriverState& state)
    {
        if(nullptr != Value)
        {
            Value->Symbols = this->Symbols;
            Value = (ExpressionASTNode*) Value->Check(state);
        }
        return this;
    }

    void BreakStatementASTNode::Emit(DriverState& state)
    {
        state.OutputStream << "\tjmp " << state.GetBreakLabel() << 'f' << endl;
    }

    void ContinueStatementASTNode::Emit(DriverState& state)
    {
        state.OutputStream << "\tjmp " << state.GetContinueLabel() << 'f' << endl;
    }

    void ExitStatementASTNode::Emit(DriverState& state)
    {
        state.OutputStream << "{{exit;}}" << endl;
    }

    void NopStatementASTNode::Emit(DriverState& state)
    {
        state.OutputStream << "\tnop" << endl;
    }

    void ReturnStatementASTNode::Emit(DriverState& state)
    {
        if(nullptr != Value)
            Value->Emit(state, "eax");
        state.OutputStream << "\tleave" << endl
                           << "\tret"   << endl;
    }
}}
