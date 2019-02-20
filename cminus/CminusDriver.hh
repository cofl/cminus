#ifndef CminusDriver_HH
    /* Preamble */
    #define CminusDriver_HH
    #include <string>
    #include <map>
    #include "CminusParser.hh"

    /* Definitions */
#define YY_DECL Cminus::parser::symbol_type yylex (CminusDriver& driver)
YY_DECL;

    /* Driver Class */
class CminusDriver
{
    public:
        CminusDriver();
        std::map<std::string, void*> Variables;
        std::string FileName;
        bool trace_parsing;
        bool trace_scanning;
        Cminus::location Location;

        int Parse(const std::string& fileName);
        void scan_begin();
        void scan_end();
};

#endif
