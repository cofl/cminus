#include "FunctionDeclarationASTNode.hpp"
#include "../asm/ASM.hpp"
#include <iostream>
#include <vector>

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

    ASTNode* FunctionDeclarationASTNode::Check(State& state)
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

    void FunctionDeclarationASTNode::Emit(State& state)
    {
        ASM::FunctionHeader(state, ID);
        Register reg;
        if(nullptr != Arguments)
        {
            int argc = Arguments->size();
            /* Is this necessary? Put them in the red zone, then increase the stack w/ variables
            if(argc > 0)
            {
                // make space to put arguments on the stack.
                int t = 4 * (argc > 6 ? 6 : argc);
                t += 16 - (t % 16);
                ASM::IncreaseStack(state, t);
            }*/
            for(int i = 0; i < Arguments->size(); i += 1)
            {
                auto arg = Arguments->at(i);
                auto data = Symbols->Variables[arg->ID];
                data->IsGlobal = false;
                // TODO: merge these as initializers into the StatementListASTNode
                switch(i)
                {
                    case 0:
                        data->StackOffset = -4;
                        if(!state.AllocRegister(RegisterIndex::EDI, RegisterLength::_32, reg))
                            throw "Register EDI already allocated!";
                        ASM::Store(state, data->StackOffset, reg);
                        state.FreeRegister(reg);
                        break;
                    case 1:
                        data->StackOffset = -8;
                        if(!state.AllocRegister(RegisterIndex::ESI, RegisterLength::_32, reg))
                            throw "Register ESI already allocated!";
                        ASM::Store(state, data->StackOffset, reg);
                        state.FreeRegister(reg);
                        break;
                    case 2:
                        data->StackOffset = -12;
                        if(!state.AllocRegister(RegisterIndex::EDX, RegisterLength::_32, reg))
                            throw "Register EDX already allocated!";
                        ASM::Store(state, data->StackOffset, reg);
                        state.FreeRegister(reg);
                        break;
                    case 3:
                        data->StackOffset = -16;
                        if(!state.AllocRegister(RegisterIndex::ECX, RegisterLength::_32, reg))
                            throw "Register ECX already allocated!";
                        ASM::Store(state, data->StackOffset, reg);
                        state.FreeRegister(reg);
                        break;
                    case 4:
                        data->StackOffset = -20;
                        if(!state.AllocRegister(RegisterIndex::R8D, RegisterLength::_32, reg))
                            throw "Register R8D already allocated!";
                        ASM::Store(state, data->StackOffset, reg);
                        state.FreeRegister(reg);
                        break;
                    case 5:
                        data->StackOffset = -24;
                        if(!state.AllocRegister(RegisterIndex::R9D, RegisterLength::_32, reg))
                            throw "Register R9D already allocated!";
                        ASM::Store(state, data->StackOffset, reg);
                        state.FreeRegister(reg);
                        break;
                    default:
                        data->StackOffset = 8 + i * 4;
                }
            }
        }
        Body->Emit(state);
        if(!state.AllocRegister(RegisterIndex::EAX, RegisterLength::_32, reg))
            throw "Register EAX already allocated!";
        ASM::Zero(state, reg);
        state.FreeRegister(reg);
        ASM::Return(state);
        ASM::FunctionFooter(state, ID);
    }
}}
