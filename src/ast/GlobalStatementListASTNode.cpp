#include "GlobalStatementListASTNode.hpp"
#include "../asm/ASM.hpp"
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

    void GlobalStatementListASTNode::Emit(State& state)
    {
        ASM::Verbatim    (state, "\t.file ");
        ASM::EncodeString(state, state.FileName);
        ASM::EndLine     (state);
        ASM::VerbatimLine(state, "\t.intel_syntax noprefix");
        ASM::Section(state, SectionType::Text);
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
        ASM::Section(state, SectionType::ROData);
        ASM::Label       (state, ".int_wformat");
        ASM::VerbatimLine(state, "\t.string \"%d\\n\"");
        ASM::Label       (state, ".int_rformat");
        ASM::VerbatimLine(state, "\t.string \"%d\"");
        for(auto it = state.StringLiterals.begin(); it != state.StringLiterals.end(); it++)
        {
            ASM::Label   (state, it->first);
            ASM::Verbatim(state, "\t.string ");
            ASM::EncodeString(state, it->second);
            ASM::EndLine (state);
        }
        ASM::Section(state, SectionType::Text);
        for(auto const& member: Members)
            member->Emit(state);
        utsname uts;
        uname(&uts);
        ASM::Verbatim(state, "\t.ident \"cllacour-CMC: (");
        ASM::Verbatim(state, uts.sysname);
        ASM::Verbatim(state, "~");
        ASM::Verbatim(state, uts.release);
        ASM::Verbatim(state, ") 0.0.4\"");
        ASM::EndLine (state);

        // GCC thinks this is a good idea, and so does the Internet
        ASM::VerbatimLine(state, "\t.section .note.GNU-stack,\"\",@progbits");
    }
}}
