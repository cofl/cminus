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
        for(auto const& member: Members)
        {
            member->Symbols = this->Symbols;
            member->Check(state);
        }
        this->Type = Members.back()->Type;
        return this;
    }

    ASTNode* StatementListASTNode::Check(DriverState& state)
    {
        if(nullptr == Symbols)
        {
            Symbols = state.SymbolStack.back();
            //Symbols = new SymbolTable(state.SymbolStack.back());
            //state.SymbolStack.push_back(Symbols);
        }
        for(auto const& member: Members)
        {
            member->Symbols = Symbols;
            member->Check(state);
        }
        state.SymbolStack.pop_back(); // get our scope off the stack
        return this;
    }

    ASTNode* VariableDeclarationASTNode::Check(DriverState& state)
    {
        auto scope = state.SymbolStack.back();
        for(auto const& member: Members)
        {
            auto data = scope->AddVariable(member->ID);
            data->Type = this->Type;
            data->InitialValue = nullptr;
            //data->ArraySize = member->IsArray ? member->ArraySize : -1;
            // TODO: initializer, array size
        }
        return this;
    }

    ASTNode* FunctionDeclarationASTNode::Check(DriverState& state)
    {
        auto scope = state.SymbolStack.back();
        auto functionScope = new SymbolTable(scope);

        auto data = scope->AddFunction(this->ID);
        data->ReturnType = this->Type;
        for(auto const& member: Arguments)
        {
            FunctionArgument argument;
            argument.Name = member->ID;
            argument.TypeID = member->Type;
            data->Arguments.push_back(argument);

            auto argumentData = functionScope->AddVariable(member->ID);
            argumentData->Type = member->Type;
        }

        state.SymbolStack.push_back(functionScope);
        Body->Symbols = functionScope;
        Body->Check(state);
        return this;
    }

    ASTNode* BinaryOperationASTNode::Check(DriverState& state)
    {
        LeftSide->Symbols = this->Symbols;
        RightSide->Symbols = this->Symbols;
        LeftSide->Check(state);
        RightSide->Check(state);
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

        }
        this->Type = Member->Type;
        return this;
    }

    ASTNode* FunctionCallASTNode::Check(DriverState& state)
    {
        auto data = Symbols->FindFunction(this->ID);
        this->Type = data->ReturnType;
        for(int i = 0; i < Arguments.size(); i += 1)
        {
            auto member = Arguments[i];
            member->Symbols = this->Symbols;
            member->Check(state);
            if(member->Type != data->Arguments[i].TypeID)
                throw std::string("Nonmatching types on argument ").append(to_string(i)); // TODO: better message
        }
        return this;
    }

    ASTNode* VariableASTNode::Check(DriverState& state)
    {
        auto data = Symbols->FindVariable(this->ID);
        this->Type = data->Type;
        return this;
    }

    ASTNode* ReturnStatementASTNode::Check(DriverState& state)
    {
        if(nullptr != Value)
        {
            Value->Symbols = this->Symbols;
            Value->Check(state);
        }
        return this;
    }

    ASTNode* WhileStatementASTNode::Check(DriverState& state)
    {
        Test->Symbols = this->Symbols;
        Body->Symbols = this->Symbols;
        Test->Check(state);
        Body->Check(state);
        return this;
    }

    ASTNode* IfStatementASTNode::Check(DriverState& state)
    {
        Test->Symbols = this->Symbols;
        IfTrue->Symbols = this->Symbols;
        Test->Check(state);
        IfTrue->Check(state);
        if(nullptr != IfFalse)
        {
            IfFalse->Symbols = this->Symbols;
            IfFalse->Check(state);
        }
        return this;
    }

    ASTNode* ReadCallASTNode::Check(DriverState& state)
    {
        Variable->Symbols = this->Symbols;
        Variable->Check(state);
        return this;
    }

    ASTNode* WriteCallASTNode::Check(DriverState& state)
    {
        Value->Symbols = this->Symbols;
        Value->Check(state);
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
