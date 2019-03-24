#ifndef FunctionDeclarationASTNode_HPP
    #define FunctionDeclarationASTNode_HPP

#include "ASTNode.hpp"
#include "StatementListASTNode.hpp"
#include "../structures/SymbolTable.hpp"
#include "../DriverState.hpp"
#include <string>

namespace Cminus { namespace AST
{
    class SimpleDeclarationASTNode
    {
        public:
            SimpleDeclarationASTNode(int typeID, std::string id);
            int Type;
            std::string ID;
    };

    class FunctionDeclarationASTNode : public ASTNode
    {
        public:
            FunctionDeclarationASTNode(std::string id, StatementListASTNode* body);
            int Type;
            std::string ID;
            std::vector<SimpleDeclarationASTNode*>* Arguments;
            StatementListASTNode* Body;

            ASTNode* Check(DriverState& state);
            void Emit(DriverState& state);
    };
}}

#endif
