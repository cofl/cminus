#include "BinaryOperationASTNode.hpp"
#include "../asm/ASM.hpp"
#include "../asm/Source.hpp"
#include "../asm/Destination.hpp"
#include <iostream>

namespace Cminus { namespace AST
{
    using std::endl;

    BinaryOperationASTNode::BinaryOperationASTNode(ASTOperationType operation, ExpressionASTNode* lhs, ExpressionASTNode* rhs)
        :   ExpressionASTNode(ASTNodeType::BinaryOperation),
            Operation(operation),
            LeftSide(lhs),
            RightSide(rhs)
    {
        // TODO: type
    }

    ASTNode* BinaryOperationASTNode::Check(State& state)
    {
        LeftSide->Symbols = this->Symbols;
        RightSide->Symbols = this->Symbols;
        LeftSide = (ExpressionASTNode*) LeftSide->Check(state);
        RightSide = (ExpressionASTNode*) RightSide->Check(state);
        // TODO: make sure left/right types are the same
        this->Type = LeftSide->Type;
        return this;
    }

    void BinaryOperationASTNode::Emit(State& state, Register& destination)
    {
        switch (Operation)
        {
            case ASTOperationType::Assign:
            {
                //TODO: check if lvalue
                RightSide->Emit(state, destination);
                Destination dest(state, LeftSide);
                ASM::Store(state, dest, destination);
            }   return;
            case ASTOperationType::Divide:
            case ASTOperationType::Modulo:
            {
                auto eax = Register::Get(RegisterIndex::EAX);
                auto edx = Register::Get(RegisterIndex::EDX);
                bool allocatedEAX = false;

                if(destination.Index == RegisterIndex::EAX)
                {
                    LeftSide->Emit(state, destination);
                    state.SaveRegisters(1, RegisterIndex::RDX);
                } else
                {
                    if(destination.Index == RegisterIndex::EDX)
                        state.SaveRegisters(1, RegisterIndex::RAX);
                    else
                        state.SaveRegisters(2, RegisterIndex::RAX, RegisterIndex::RDX);
                    if(!state.AllocRegister(RegisterIndex::EAX, RegisterLength::_32, eax))
                        throw "Register EAX already allocated!";
                    else
                        allocatedEAX = true;
                    LeftSide->Emit(state, eax);
                }

                // manually reserve edx if it isn't already
                auto edxs = state.RegisterStatus(edx);
                state.SetRegisterStatus(edx, false);
                ASM::Zero(state, edx);

                auto rsr = state.AllocRegister(RegisterLength::_32);
                RightSide->Emit(state, rsr);
                ASM::Operation(state, "idiv", rsr);

                if (Operation == ASTOperationType::Divide)
                {
                    if (destination.Index != RegisterIndex::EAX)
                        ASM::Move(state, destination, eax);
                } else
                {
                    if (destination.Index != RegisterIndex::EDX)
                        ASM::Move(state, destination, edx);
                }

                // cleanup, restore edx state
                if(allocatedEAX)
                    state.FreeRegister(eax);
                state.FreeRegister(rsr);
                state.RestoreRegisters();
                state.SetRegisterStatus(edx, edxs);
            }   return;
            default:
                // fall through to the other cases
                break;
        }

        LeftSide->Emit(state, destination);

        Source source(state, RightSide);
        switch(Operation)
        {
            case ASTOperationType::Exponentiation:
                state.OutputStream << " ** "; // TODO
                break;
            case ASTOperationType::Multiply:    ASM::Operation(state, "imul", destination, source); break;
            case ASTOperationType::Add:         ASM::Operation(state, "add", destination, source); break;
            case ASTOperationType::Subtract:    ASM::Operation(state, "sub", destination, source); break;
            case ASTOperationType::RightShift:  ASM::Operation(state, "shr", destination, source); break;
            case ASTOperationType::LeftShift:   ASM::Operation(state, "shl", destination, source); break;
            case ASTOperationType::BinaryXor:   ASM::Operation(state, "xor", destination, source); break;
            case ASTOperationType::BinaryOr:    ASM::Operation(state, "or", destination, source); break;
            case ASTOperationType::BinaryAnd:   ASM::Operation(state, "and", destination, source); break;
            case ASTOperationType::LogicalOr:
                ASM::Operation(state, "or", destination, source);
                ASM::CmpAndSet(state, "setnz", destination);
                break;
            case ASTOperationType::LogicalAnd:  ASM::TestAndSet(state, "setnz", destination, source); break;
            case ASTOperationType::TestGT:      ASM::CmpAndSet(state, "setg", destination, source); break;
            case ASTOperationType::TestGE:      ASM::CmpAndSet(state, "setge", destination, source); break;
            case ASTOperationType::TestLT:      ASM::CmpAndSet(state, "setl", destination, source); break;
            case ASTOperationType::TestLE:      ASM::CmpAndSet(state, "setle", destination, source); break;
            case ASTOperationType::TestNE:      ASM::CmpAndSet(state, "setnz", destination, source); break;
            case ASTOperationType::TestEQ:      ASM::CmpAndSet(state, "setz", destination, source); break;
            default:
                state.OutputStream << "{{unrecognized binary operation}}" << endl;
        }
    }
}}
