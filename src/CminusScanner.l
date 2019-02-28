WS  [ \t\r]
HEX [a-fA-F0-9]
String  \"[^\\\"]*(\\([\\\"abfnrtv'?0]|[xX][a-fA-F0-9][a-fA-F0-9])[^\\\"]*)*\"

%{
#include<iostream>
#include<sstream>
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
using Parser = Cminus::parser;
%}

%option c++ yyclass="Cminus::Scanner" noyywrap
%option 8bit ecs meta-ecs stack
%option debug nodefault warn
%option noinput nounput

%x BLOCK_COMMENT
%x LINE_COMMENT
%x STRING
%x STRING_ESCAPE
%%
%{
    /* run inside the function */
Cminus::location& location = DriverInstance.Location;
location.step();

ostringstream tempString;
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
int      return Parser::make_INTEGER(location);
while    return Parser::make_WHILE(location);
else     return Parser::make_ELSE(location);
exit     return Parser::make_EXIT(location);
if       return Parser::make_IF(location);
read     return Parser::make_READ(location);
write    return Parser::make_WRITE(location);
return   return Parser::make_RETURN(location);

    /* Operators */
"~"      return Parser::make_BNOT(location);
"^"      return Parser::make_BXOR(location);
">>"     return Parser::make_RSHIFT(location);
"<<"     return Parser::make_LSHIFT(location);
"!"      return Parser::make_NOT(location);
"||"     return Parser::make_OR(location);
"|"      return Parser::make_BOR(location);
"&&"     return Parser::make_AND(location);
"&"      return Parser::make_BAND(location);
"\<="    return Parser::make_LE(location);
"<"      return Parser::make_LT(location);
">="     return Parser::make_GE(location);
">"      return Parser::make_GT(location);
"=="     return Parser::make_EQ(location);
"!="     return Parser::make_NE(location);
"="      return Parser::make_ASSIGN(location);
"*="     return Parser::make_MULT_ASSIGN(location);
"/="     return Parser::make_DIV_ASSIGN(location);
"+="     return Parser::make_ADD_ASSIGN(location);
"-="     return Parser::make_SUB_ASSIGN(location);
"%="     return Parser::make_MOD_ASSIGN(location);
"^="     return Parser::make_BXOR_ASSIGN(location);
"&="     return Parser::make_BAND_ASSIGN(location);
"|="     return Parser::make_BOR_ASSIGN(location);
"**="    return Parser::make_EXPNT_ASSIGN(location);
"&&="    return Parser::make_LAND_ASSIGN(location);
"||="    return Parser::make_LOR_ASSIGN(location);
"<<="    return Parser::make_LSHIFT_ASSIGN(location);
">>="    return Parser::make_RSHIFT_ASSIGN(location);
";"      return Parser::make_SEMICOLON(location);
"{"      return Parser::make_LBRACE(location);
"}"      return Parser::make_RBRACE(location);
"["      return Parser::make_LBRACKET(location);
"]"      return Parser::make_RBRACKET(location);
"("      return Parser::make_LPAREN(location);
")"      return Parser::make_RPAREN(location);
"+"      return Parser::make_PLUS(location);
"-"      return Parser::make_MINUS(location);
"**"     return Parser::make_EXPONENT(location);
"*"      return Parser::make_TIMES(location);
"/"      return Parser::make_DIVIDE(location);
"%"      return Parser::make_MODULO(location);
","      return Parser::make_COMMA(location);

    /* Literals */
[a-zA-Z_][a-zA-Z0-9_]* return Parser::make_IDENTIFIER(YYText(), location);
0[xX][0-9a-fA-F]+      return Parser::make_INTCON(stoi(YYText(), nullptr, 16), location);
[0-9]+                 return Parser::make_INTCON(stoi(YYText(), nullptr, 10), location);

    /* Strings */
\"                          BEGIN(STRING);
<STRING>\\                  BEGIN(STRING_ESCAPE);
<STRING_ESCAPE>[xX]{HEX}{2} tempString << (char) stoi(&(YYText()[1]), nullptr, 16); BEGIN(STRING);
<STRING_ESCAPE>[0-7]{3}     tempString << (char) stoi(YYText(), nullptr, 8); BEGIN(STRING);
<STRING_ESCAPE>\\           tempString << '\\'; BEGIN(STRING);
<STRING_ESCAPE>\"           tempString << '"';  BEGIN(STRING);
<STRING_ESCAPE>\'           tempString << '\''; BEGIN(STRING);
<STRING_ESCAPE>\?           tempString << '?';  BEGIN(STRING);
<STRING_ESCAPE>a            tempString << '\a'; BEGIN(STRING);
<STRING_ESCAPE>b            tempString << '\b'; BEGIN(STRING);
<STRING_ESCAPE>f            tempString << '\f'; BEGIN(STRING);
<STRING_ESCAPE>v            tempString << '\v'; BEGIN(STRING);
<STRING_ESCAPE>n            tempString << '\n'; BEGIN(STRING);
<STRING_ESCAPE>\n           location.lines(1); tempString << '\n'; BEGIN(STRING);
<STRING_ESCAPE>\r\n         location.lines(1); tempString << "\r\n"; BEGIN(STRING); // To support Windows editors.
<STRING_ESCAPE>r            tempString << '\r'; BEGIN(STRING);
<STRING_ESCAPE>t            tempString << '\t'; BEGIN(STRING);
<STRING_ESCAPE>0            tempString << '\0'; BEGIN(STRING);
<STRING_ESCAPE>.            {
    location.step(); location.begin.column -= 1; location.end.column += 1;
    throw Parser::syntax_error(location, "Invalid escape sequence: \\" + std::string(YYText()));
}
<STRING>\n                  {
    location.step(); location.begin.column -= 1; location.lines(1);
    throw Parser::syntax_error(location, "Illegal unescaped newline literal in string.");
}
<STRING>\"                  BEGIN(INITIAL); return Parser::make_STRING(tempString.str(), location);
<STRING>.                   tempString << YYText();

    /* Catch-all */
.   throw Parser::syntax_error(location, "Invalid charater: " + std::string(YYText()));
<<EOF>>                return Parser::make_END_OF_FILE(location);
%%

Cminus::Scanner::Scanner(std::istream& in) : yyFlexLexer(&in)
{
    // nop
}
