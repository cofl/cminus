WS  [ \t\r]
HEX [a-fA-F0-9]
String  \"[^\\\"]*(\\([\\\"abfnrtv'?0]|[xX][a-fA-F0-9][a-fA-F0-9])[^\\\"]*)*\"

%{
#include<iostream>
#include<cerrno>
#include<climits>
#include<cstdlib>
#include<cstdio>
#include<cstring>
#include<string>
#include "CminusParser.hpp"
#include "CminusDriver.hpp"
#undef YY_DECL
#include "CminusScanner.hpp"

#define STEP() location.step()
#define STEP_LINES(x) { location.lines((x)); location.step(); }

/* This is run every time a pattern is matched */
#define YY_USER_ACTION location.columns(YYLeng());

using namespace std;

Cminus::parser::symbol_type make_STRING(const std::string& str, const Cminus::parser::location_type& location);
Cminus::parser::symbol_type make_INTCON_Base10(const std::string& str, const Cminus::parser::location_type& location);
Cminus::parser::symbol_type make_INTCON_Base16(const std::string& str, const Cminus::parser::location_type& location);
%}

%option c++ yyclass="Cminus::Scanner" noyywrap
%option 8bit ecs meta-ecs stack
%option debug nodefault warn
%option noinput nounput

%x BLOCK_COMMENT
%x LINE_COMMENT
%%
%{
    /* run inside the function */
Cminus::location& location = DriverInstance.Location;
location.step();
%}
    /* Skip blanks, comments */
{WS}+               location.step();
\n+                 STEP_LINES(YYLeng());
"//"                location.step(); BEGIN(LINE_COMMENT);

"/*"                location.step(); BEGIN(BLOCK_COMMENT);
<LINE_COMMENT>\n+   STEP_LINES(YYLeng()); BEGIN(INITIAL);
<LINE_COMMENT>.     location.step();

<BLOCK_COMMENT>"*/" location.step(); BEGIN(INITIAL);
<BLOCK_COMMENT>\n+  STEP_LINES(YYLeng());
<BLOCK_COMMENT>.    location.step();

    /* Alpha Constants */
int      return Cminus::parser::make_INTEGER(location);
while    return Cminus::parser::make_WHILE(location);
else     return Cminus::parser::make_ELSE(location);
exit     return Cminus::parser::make_EXIT(location);
if       return Cminus::parser::make_IF(location);
read     return Cminus::parser::make_READ(location);
getc     return Cminus::parser::make_GETC(location);
write    return Cminus::parser::make_WRITE(location);
print    return Cminus::parser::make_PRINT(location);
len      return Cminus::parser::make_LEN_CALL(location);
return   return Cminus::parser::make_RETURN(location);

    /* Operators */
"~"      return Cminus::parser::make_BNOT(location);
"^"      return Cminus::parser::make_BXOR(location);
">>"     return Cminus::parser::make_RSHIFT(location);
"<<"     return Cminus::parser::make_LSHIFT(location);
"!"      return Cminus::parser::make_NOT(location);
"||"     return Cminus::parser::make_OR(location);
"|"      return Cminus::parser::make_BOR(location);
"&&"     return Cminus::parser::make_AND(location);
"&"      return Cminus::parser::make_BAND(location);
"\<="    return Cminus::parser::make_LE(location);
"<"      return Cminus::parser::make_LT(location);
">="     return Cminus::parser::make_GE(location);
">"      return Cminus::parser::make_GT(location);
"=="     return Cminus::parser::make_EQ(location);
"!="     return Cminus::parser::make_NE(location);
"="      return Cminus::parser::make_ASSIGN(location);
"*="     return Cminus::parser::make_MULT_ASSIGN(location);
"/="     return Cminus::parser::make_DIV_ASSIGN(location);
"+="     return Cminus::parser::make_ADD_ASSIGN(location);
"-="     return Cminus::parser::make_SUB_ASSIGN(location);
"%="     return Cminus::parser::make_MOD_ASSIGN(location);
"^="     return Cminus::parser::make_BXOR_ASSIGN(location);
"&="     return Cminus::parser::make_BAND_ASSIGN(location);
"|="     return Cminus::parser::make_BOR_ASSIGN(location);
"**="    return Cminus::parser::make_EXPNT_ASSIGN(location);
"&&="    return Cminus::parser::make_LAND_ASSIGN(location);
"||="    return Cminus::parser::make_LOR_ASSIGN(location);
"<<="    return Cminus::parser::make_LSHIFT_ASSIGN(location);
">>="    return Cminus::parser::make_RSHIFT_ASSIGN(location);
";"      return Cminus::parser::make_SEMICOLON(location);
"{"      return Cminus::parser::make_LBRACE(location);
"}"      return Cminus::parser::make_RBRACE(location);
"["      return Cminus::parser::make_LBRACKET(location);
"]"      return Cminus::parser::make_RBRACKET(location);
"("      return Cminus::parser::make_LPAREN(location);
")"      return Cminus::parser::make_RPAREN(location);
"+"      return Cminus::parser::make_PLUS(location);
"-"      return Cminus::parser::make_MINUS(location);
"**"     return Cminus::parser::make_EXPONENT(location);
"*"      return Cminus::parser::make_TIMES(location);
"/"      return Cminus::parser::make_DIVIDE(location);
"%"      return Cminus::parser::make_MODULO(location);
","      return Cminus::parser::make_COMMA(location);

    /* Literals */
{String}               return make_STRING(YYText(), location);
[a-zA-Z_][a-zA-Z0-9_]* return Cminus::parser::make_IDENTIFIER(YYText(), location);
0[xX][0-9a-fA-F]+      return make_INTCON_Base16(YYText(), location);
[0-9]+                 return make_INTCON_Base10(YYText(), location);

    /* Catch-all */
.   { throw Cminus::parser::syntax_error(location, "Invalid charater: " + std::string(YYText())); }
<<EOF>>                return Cminus::parser::make_END_OF_FILE(location);
%%

static int hc2i(char c)
{
    if(c >= 'a')
        return c - 'a' + 10;
    if(c >= 'A')
        return c - 'A' + 10;
    return c - '0';
}

Cminus::parser::symbol_type make_STRING(const std::string& str, const Cminus::parser::location_type& location)
{
    int i = 0, j = 0, length = str.length();
    std::string text = str;
    char* temp = ((char*) text.c_str()) + 1;
    while(i < length && temp[i] != '"')
    {
        if(temp[i] == '\\')
        {
            i += 1;
            switch(temp[i])
            {
                case '\\':
                case '"':
                case '\'':
                case '?':
                    temp[j] = temp[i];
                    break;
                case 'a':
                    temp[j] = '\a';
                    break;
                case 'b':
                    temp[j] = '\b';
                    break;
                case 'f':
                    temp[j] = '\f';
                    break;
                case 'v':
                    temp[j] = '\v';
                    break;
                    temp[j] = '\'';
                case 'n':
                    temp[j] = '\n';
                    break;
                case 'r':
                    temp[j] = '\r';
                    break;
                case 't':
                    temp[j] = '\t';
                    break;
                case '0':
                    temp[j] = '\0';
                    break;
                case 'x':
                case 'X':
                    temp[j] = hc2i(temp[i + 1]) * 0x10 + hc2i(temp[i + 2]);
                    i += 2;
                    break;
                default:
                    fprintf(stderr, "Scanner: Unrecognized escaped character '%c'\n", temp[i]);
                    break;
            }
        }
        i += 1;
        j += 1;
    }
    temp[j] = '\0';
    return Cminus::parser::make_STRING(std::string(temp), location);
}

Cminus::parser::symbol_type make_INTCON_Base10(const std::string& str, const Cminus::parser::location_type& location)
{
    int temp = atoi(str.c_str());
    std::cerr << "Parsed constant " << temp << std::endl;
    return Cminus::parser::make_INTCON(temp, location);
}

Cminus::parser::symbol_type make_INTCON_Base16(const std::string& str, const Cminus::parser::location_type& location)
{
    int temp;
    sscanf(str.c_str(), "%x", &temp);
    std::cerr << "Parsed constant " << temp << std::endl;
    return Cminus::parser::make_INTCON(temp, location);
}

Cminus::Scanner::Scanner(std::istream& in) : yyFlexLexer(&in)
{
    // nop
}
