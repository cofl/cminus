#include "GlobalStatementListASTNode.hpp"
#include <iostream>
#include <sys/utsname.h>

namespace Cminus { namespace AST
{
    using namespace Cminus::Structures;
    using std::endl;

    GlobalStatementListASTNode::GlobalStatementListASTNode()
        :   StatementListASTNode()
    {
        // nop
    }

    void GlobalStatementListASTNode::Emit(DriverState& state)
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
        for(auto const& member: Members)
            member->Emit(state);
        utsname uts;
        uname(&uts);
        state.OutputStream << "\t.ident \"cllacour-CMC: ("
                            << uts.sysname << "~" << uts.release
                            << ") 0.0.3\"" << endl;
        // GCC thinks this is a good idea, and so does the internet
        state.OutputStream << "\t.section\t.note.GNU-stack,\"\",@progbits" << endl;
    }
}}
