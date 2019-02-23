#ifndef CminusScanner_HPP
    /* Preamble */
    #define CminusScanner_HPP
    #if !defined(yyFlexLexerOnce)
        #include <FlexLexer.h>
    #endif
    #include "CminusDriver.hpp"
    #include "CminusParser.hpp"

    /* Definitions */
#define YY_DECL Cminus::parser::symbol_type Cminus::Scanner::yylex(Cminus::Driver& DriverInstance)

namespace Cminus
{
    class Scanner : public yyFlexLexer
    {
        public:
            Scanner(std::istream& in);

            using FlexLexer::yylex;

            parser::symbol_type yylex(Cminus::Driver& DriverInstance);
    };
}
#endif
