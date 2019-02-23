#ifndef ASTToken_HPP
    #define ASTToken_HPP
#include "SymbolTable.hpp"
#include <memory>

namespace Cminus { namespace Structures
{
    class ASTToken
    {
        public:
            bool IsConstant;
            std::shared_ptr<SymbolTableFrame> Frame;
    };

    class ValueToken : public ASTToken
    {

    };

    class OperationToken : public ASTToken
    {
        public:
            enum class OperationType
            {
                Add,
                Subtract,
                Multiply,
                Divide,

            };
    };
}}

#endif
