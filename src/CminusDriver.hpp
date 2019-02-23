#ifndef CminusDriver_HPP
    /* Preamble */
    #define CminusDriver_HPP
    #include <string>
    #include <map>
    #include "CminusParser.hpp"

    /* Definitions */
#define YY_DECL Cminus::parser::symbol_type yylex(CminusDriver& driver)

    /* Driver Class */
class CminusDriver
{
    public:
        CminusDriver();
        std::map<std::string, int*> Variables;
        std::string FileName;
        bool trace_parsing;
        bool trace_scanning;
        Cminus::location Location;

        int Parse(const std::string& fileName);
        void scan_begin();
        void scan_end();
};

YY_DECL;
#endif
