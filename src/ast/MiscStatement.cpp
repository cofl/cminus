#include "MiscStatement.hpp"
#include "../asm/ASM.hpp"
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

    ASTNode* BreakStatementASTNode::Check(State& state)
    {
        if(state.BreakLabels.size() == 0)
            throw "Illegal break statement.";
        return this;
    }

    ASTNode* ContinueStatementASTNode::Check(State& state)
    {
        if(state.ContinueLabels.size() == 0)
            throw "Illegal continue statement.";
        return this;
    }

    ASTNode* ReturnStatementASTNode::Check(State& state)
    {
        if(nullptr != Value)
        {
            Value->Symbols = this->Symbols;
            Value = (ExpressionASTNode*) Value->Check(state);
        }
        return this;
    }

    void BreakStatementASTNode::Emit(State& state)
    {
        ASM::JumpForward(state, state.GetBreakLabel());
    }

    void ContinueStatementASTNode::Emit(State& state)
    {
        ASM::JumpForward(state, state.GetContinueLabel());
    }

    void ExitStatementASTNode::Emit(State& state)
    {
        state.OutputStream << "{{exit;}}" << endl;
    }

    void NopStatementASTNode::Emit(State& state)
    {
        ASM::VerbatimLine(state, "\tnop");
    }

    void ReturnStatementASTNode::Emit(State& state)
    {
        if(nullptr != Value)
        {
            Register eax;
            if(!state.AllocRegister(RegisterIndex::EAX, RegisterLength::_32, eax))
                throw "Register EAX already allocated!";
            Value->Emit(state, eax);
            state.FreeRegister(eax);
        }
        ASM::Return(state);
    }
}}
