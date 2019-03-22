#include "../DriverState.hpp"
#include "ASTNode.hpp"
#include "SymbolTable.hpp"
#include <iostream>
#include <string>

namespace Cminus { namespace Structures
{
    using namespace std;

    ASTNode* ASTNode::Check(DriverState& state)
    {
        // do-nothing as a default
        return this;
    }

    ASTNode* ExpressionListASTNode::Check(DriverState& state)
    {
        int len = Members.size();
        for(int i = 0; i < len; i += 1)
        {
            auto member = Members[i];
            member->Symbols = this->Symbols;
            Members[i] = (ExpressionASTNode*) member->Check(state);
        }
        this->Type = Members.back()->Type;
        return this;
    }

    ASTNode* StatementListASTNode::Check(DriverState& state)
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

    ASTNode* VariableDeclarationASTNode::Check(DriverState& state)
    {
        auto scope = state.SymbolStack.back();
        int len = Members.size();
        for(int i = 0; i < len; i += 1)
        {
            auto member = Members[i];
            auto data = scope->AddVariable(member->ID);
            data->Type = this->Type;
            data->InitialValue = nullptr;
            data->ArraySize = member->ArraySize;
            // TODO: initializer
        }
        return this;
    }

    ASTNode* FunctionDeclarationASTNode::Check(DriverState& state)
    {
        auto scope = state.SymbolStack.back();
        Symbols = new SymbolTable(scope);

        auto data = scope->AddFunction(this->ID);
        data->ReturnType = this->Type;
        if(nullptr != Arguments)
        {
            int len = Arguments->size();
            for(int i = 0; i < len; i += 1)
            {
                auto member = Arguments->at(i);
                FunctionArgument argument;
                argument.Name = member->ID;
                argument.TypeID = member->Type;
                data->Arguments.push_back(argument);

                auto argumentData = Symbols->AddVariable(member->ID);
                argumentData->Type = member->Type;
            }
        }
        state.SymbolStack.push_back(Symbols);
        Body->Symbols = Symbols;
        Body = (StatementListASTNode*) Body->Check(state);
        return this;
    }

    ASTNode* BinaryOperationASTNode::Check(DriverState& state)
    {
        LeftSide->Symbols = this->Symbols;
        RightSide->Symbols = this->Symbols;
        LeftSide = (ExpressionASTNode*) LeftSide->Check(state);
        RightSide = (ExpressionASTNode*) RightSide->Check(state);
        // TODO: make sure left/right types are the same
        this->Type = LeftSide->Type;
        return this;
    }

    ASTNode* UnaryOperationASTNode::Check(DriverState& state)
    {
        Member->Symbols = this->Symbols;
        Member->Check(state);
        if(Member->IsConstant)
        {
            // TODO
        }
        this->Type = Member->Type;
        return this;
    }

    ASTNode* FunctionCallASTNode::Check(DriverState& state)
    {
        auto data = Symbols->FindFunction(this->ID);
        if(data == nullptr)
        {
            // if we haven't declared it, assume it's in the standard library.
            this->Type = state.GetTypeID("int");
            this->ID = this->ID.append("@PLT");
        } else
        {
            this->Type = data->ReturnType;
        }
        for(int i = 0; i < Arguments.size(); i += 1)
        {
            auto member = Arguments[i];
            member->Symbols = this->Symbols;
            Arguments[i] = (ExpressionASTNode*) member->Check(state);
            if(nullptr != data && member->Type != data->Arguments[i].TypeID)
                throw std::string("Nonmatching types on argument ").append(to_string(i)); // TODO: better message
        }
        return this;
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

    ASTNode* ReturnStatementASTNode::Check(DriverState& state)
    {
        if(nullptr != Value)
        {
            Value->Symbols = this->Symbols;
            Value = (ExpressionASTNode*) Value->Check(state);
        }
        return this;
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

    ASTNode* ForStatementASTNode::Check(DriverState& state)
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

    ASTNode* IfStatementASTNode::Check(DriverState& state)
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

    ASTNode* ReadCallASTNode::Check(DriverState& state)
    {
        Variable->Symbols = this->Symbols;
        Variable = (ExpressionASTNode*) Variable->Check(state);
        return this;
    }

    ASTNode* WriteCallASTNode::Check(DriverState& state)
    {
        Value->Symbols = this->Symbols;
        Value = (ExpressionASTNode*) Value->Check(state);
        return this;
    }

    ASTNode* IntegerLiteralASTNode::Check(DriverState& state)
    {
        return this;
    }

    ASTNode* StringLiteralASTNode::Check(DriverState& state)
    {
        auto id = state.AddStringConstant(Value);
        Value = std::string(".string_constant").append(to_string(id));
        return this;
    }
}}
