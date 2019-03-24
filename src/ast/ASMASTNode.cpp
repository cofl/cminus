#include "ASMASTNode.hpp"
#include <iostream>
#include <regex>

namespace Cminus { namespace AST
{
    using std::endl;

    ASMASTNode::ASMASTNode(string value)
        :   ASTNode(ASTNodeType::ASM),
            Value(value)
    {
        // nop
    }

    void ASMASTNode::Emit(DriverState& state)
    {
        smatch rgxMatch;
        regex varExpr("\\$([a-zA-Z_][a-zA-Z0-9_]*)");
        while(regex_search(Value, rgxMatch, varExpr))
        {
            auto varID = rgxMatch[1].str();
            auto data = Symbols->FindVariable(varID);
            state.OutputStream << rgxMatch.prefix().str();
            if(data->IsGlobal)
                state.OutputStream << data->GlobalLocation << "[rip]";
            else
                state.OutputStream << data->StackOffset << "[rbp]";
            Value = rgxMatch.suffix().str();
        }
        state.OutputStream << Value << endl;
    }
}}
