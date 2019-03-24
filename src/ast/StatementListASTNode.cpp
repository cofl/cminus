#include "StatementListASTNode.hpp"
#include <iostream>
#include <sys/utsname.h>

namespace Cminus { namespace AST
{
    using namespace Cminus::Structures;
    using std::endl;

    StatementListASTNode::StatementListASTNode()
        :   ASTNode(ASTNodeType::StatementList),
            IsGlobal(false)
    {
        // nop
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

    void StatementListASTNode::Emit(DriverState& state)
    {
        if(IsGlobal)
        {
            state.OutputStream << "\t.file \"" << *(state.FileName) << "\"" << endl
                               << "\t.intel_syntax noprefix /* I have to be difficult because I like to read */" << endl
                               << "\t.text"             << endl;
            for(auto it = Symbols->Variables.begin(); it != Symbols->Variables.end(); it++)
            {
                auto name = it->first;
                auto data = it->second;
                data->IsGlobal = true;
                if(nullptr == data->InitialValue)
                {
                    data->GlobalLocation = name;
                    state.OutputStream << "\t.comm\t" << name << "," << (4 * data->ArraySize) << ",4" << endl;
                    continue;
                }
                // TODO: emit constant code
            }
            state.OutputStream << "\t.section .rodata"  << endl
                               << ".int_wformat:"       << endl
                               << "\t.string \"%d\\n\"" << endl
                               << ".int_rformat:"       << endl
                               << "\t.string \"%d\""    << endl;
            for(int i = 0; i < state.StringConstants.size(); i += 1)
            {
                state.OutputStream << ".string_constant" << i << ":" << endl
                                   << "\t.string \"";
                auto string = state.StringConstants[i];
                auto length = string.length();
                for(int i = 0; i < length; i += 1)
                {
                    auto chr = string[i];
                    switch(chr)
                    {
                        case '\b': state.OutputStream << "\\b"; break;
                        case '\f': state.OutputStream << "\\f"; break;
                        case '\n': state.OutputStream << "\\n"; break;
                        case '\r': state.OutputStream << "\\r"; break;
                        case '\t': state.OutputStream << "\\t"; break;
                        case '\\': state.OutputStream << "\\\\"; break;
                        case '\"': state.OutputStream << "\\\""; break;
                        default:
                            if(chr >= ' ' && chr <= '~' && chr != '\\' && chr != '\"')
                            {
                                state.OutputStream << chr;
                            } else
                            {
                                char upper = (chr & 0xF0) >> 4;
                                char lower = chr & 0x0F;
                                state.OutputStream << "\\x"
                                                   << (upper > 9 ? upper + 'A' : upper + '0')
                                                   << (lower > 9 ? lower + 'A' : lower + '0');
                            }
                            break;
                    }
                }
                state.OutputStream << "\"" << endl;
            }
            state.OutputStream << "\t.text" << endl;
        } else
        {
            if(!Symbols->Variables.empty())
            {
                // reserve space on the stack
                state.OutputStream << "\tsub rsp, " << Symbols->GetAlignedSize() << endl;
                int i = 1;
                for(auto it = Symbols->Variables.begin(); it != Symbols->Variables.end(); it++, i += 1)
                {
                    auto data = it->second;
                    data->IsGlobal = false;
                    data->StackOffset = i * data->ArraySize * -4 - state.BaseOffset;
                    if(nullptr != data->InitialValue)
                    {
                        // TODO: emit initializer code
                    }
                }
                state.BaseOffset += Symbols->GetAlignedSize();
            }
        }
        for(auto const& member: Members)
            member->Emit(state);
        if(IsGlobal)
        {
            utsname uts;
            uname(&uts);
            state.OutputStream << "\t.ident \"cllacour-CMC: ("
                               << uts.sysname << "~" << uts.release
                               << ") 0.0.3\"" << endl;
            // GCC thinks this is a good idea, and so does the internet
            state.OutputStream << "\t.section\t.note.GNU-stack,\"\",@progbits" << endl;
        } else if(!Symbols->Variables.empty())
        {
            state.BaseOffset -= Symbols->GetAlignedSize();
        }
    }
}}
