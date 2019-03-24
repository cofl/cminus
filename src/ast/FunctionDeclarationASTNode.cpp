#include "FunctionDeclarationASTNode.hpp"
#include <iostream>

namespace Cminus { namespace AST
{
    using namespace Cminus::Structures;
    using std::endl;

    FunctionDeclarationASTNode::FunctionDeclarationASTNode(std::string id, StatementListASTNode* body)
        :   ASTNode(ASTNodeType::FunctionDeclaration),
            ID(id),
            Body(body),
            Arguments(nullptr)
    {
        // nop
    }

    SimpleDeclarationASTNode::SimpleDeclarationASTNode(int typeID, string id)
        :   Type(typeID),
            ID(id)
    {
        // nop
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

    void FunctionDeclarationASTNode::Emit(DriverState& state)
    {
        int prevBaseOffset = state.BaseOffset;
        state.BaseOffset = 0;
        state.OutputStream << "\t.global\t" << ID                << endl
                           << "\t.type\t" << ID << ", @function" << endl
                           << ID << ":"                          << endl
                           << "\tpush rbp"                       << endl
                           << "\tmov rbp, rsp"                   << endl;
        if(nullptr != Arguments)
        {
            int argc = Arguments->size();
            if(argc > 0)
            {
                // make space to put arguments on the stack.
                int t = 4 * (argc > 6 ? 6 : argc);
                t += 16 - (t % 16);
                state.OutputStream << "\tsub rsp," << t << endl;
                state.BaseOffset = +t;
            }
            for(int i = 0; i < Arguments->size(); i += 1)
            {
                auto arg = Arguments->at(i);
                auto data = Symbols->Variables[arg->ID];
                data->IsGlobal = false;
                switch(i)
                {
                    case 0:
                        data->StackOffset = -4;
                        state.OutputStream << "\tmov -4[rbp],edi" << endl;
                        break;
                    case 1:
                        data->StackOffset = -8;
                        state.OutputStream << "\tmov -8[rbp],esi" << endl;
                        break;
                    case 2:
                        data->StackOffset = -12;
                        state.OutputStream << "\tmov -12[rbp],edx" << endl;
                        break;
                    case 3:
                        data->StackOffset = -16;
                        state.OutputStream << "\tmov -16[rbp],ecx" << endl;
                        break;
                    case 4:
                        data->StackOffset = -20;
                        state.OutputStream << "\tmov -20[rbp],r8d" << endl;
                        break;
                    case 5:
                        data->StackOffset = -24;
                        state.OutputStream << "\tmov -24[rbp],r9d" << endl;
                        break;
                    default:
                        data->StackOffset = 8 + i * 4;
                }
            }
        }
        Body->Emit(state);
        state.OutputStream << "\tmov eax, 0" << endl
                           << "\tleave"      << endl
                           << "\tret"        << endl
                           << "\t.size\t" << ID << ", .-" << ID << endl;
        state.BaseOffset = prevBaseOffset;
    }
}}
