WS  [ \t\r]
HEX [a-fA-F0-9]
String  \"[^\\\"]*(\\([\\\"abfnrtv'?0]|[xX][a-fA-F0-9][a-fA-F0-9])[^\\\"]*)*\"

%{
#include<cerrno>
#include<climits>
#include<cstdlib>
#include<cstring>
#include<string>
#include "CminusParser.hh"

#define STEP() location.step()
#define STEP_LINES(x) { location.lines((x)) location.step(); }

/* This is run every time a pattern is matched */
#define YY_USER_ACTION location.columns(yyleng);

using namespace std;
%}

%option noyywrap c++
%option nounput noinput batch debug
%option prefix="Cminus_"

%x BLOCK_COMMENT
%x LINE_COMMENT

%%
%{
    /* run inside the function */
Cminus::location& location = DriverInstance.location;
location.step();
%}
    /* Skip blanks, comments */
{WS}+               location.step();
\n+                 STEP_LINES(Cminus_leng);
"//"                location.step(); BEGIN(LINE_COMMENT);

"/*"                location.step(); BEGIN(BLOCK_COMMENT);
<LINE_COMMENT>\n+   STEP_LINES(Cminus_leng); BEGIN(INITIAL);
<LINE_COMMENT>.     location.step();

<BLOCK_COMMENT>"*/" location.step(); BEGIN(INITIAL);
<BLOCK_COMMENT>\n+  STEP_LINES(Cminus_leng);
<BLOCK_COMMENT>.    location.step();

    /* Alpha Constants */
int      return INTEGER;
while    return WHILE;
else     return ELSE;
exit     return EXIT;
if       return IF;
read     return READ;
getc     return GETC;
write    return WRITE;
print    return PRINT;
len      return LEN_CALL;
return   return RETURN;

    /* Operators */
"~"      return BNOT;
"^"      return BXOR;
">>"     return RSHIFT;
"<<"     return LSHIFT;
"!"      return NOT;
"||"     return OR;
"|"      return BOR;
"&&"     return AND;
"&"      return BAND;
"\<="    return LE;
"<"      return LT;
">="     return GE;
">"      return GT;
"=="     return EQ;
"!="     return NE;
"="      return ASSIGN;
"*="     return MULT_ASSIGN;
"/="     return DIV_ASSIGN;
"+="     return ADD_ASSIGN;
"-="     return SUB_ASSIGN;
"%="     return MOD_ASSIGN;
"^="     return BXOR_ASSIGN;
"&="     return BAND_ASSIGN;
"|="     return BOR_ASSIGN;
"**="    return EXPNT_ASSIGN;
"&&="    return LAND_ASSIGN;
"||="    return LOR_ASSIGN;
"<<="    return LSHIFT_ASSIGN;
">>="    return RSHIFT_ASSIGN;
";"      return SEMICOLON;
"{"      return LBRACE;
"}"      return RBRACE;
"["      return LBRACKET;
"]"      return RBRACKET;
"("      return LPAREN;
")"      return RPAREN;
"+"      return PLUS;
"-"      return MINUS;
"**"     return EXPONENT;
"*"      return TIMES;
"/"      return DIVIDE;
"%"      return MODULO;
","      return COMMA;

    /* Literals */
{String}               return emitString(Cminus_text);
[a-zA-Z_][a-zA-Z0-9_]* return emitIdentifier(Cminus_text);
0[xX][0-9a-fA-F]+      return emitInteger(Literal_Base16, Cminus_text);
[0-9]+                 return emitInteger(Literal_Base10, Cminus_text);

    /* Catch-all */
.   { throw Cminus::parser::syntax_error(loc, "Invalid charater: " + std::string(Cminus_text)); }
<<EOF>> { /* TODO */ }
%%

void CminusDriver::scan_begin()
{
    yy_flex_debug = trace_scanning;
    if (file.empty() || file == "-")
    {
        Cminus_in = stdin;
    } else if (!(Cminus_in = fopen(file.c_str(), "rb")))
    {
        cerr << "Cannot open " << file ": " << sterror(errno) << "\n";
        exit(EXIT_FAILURE);
    }
}

void CminusDriver::scan_end()
{
    fclose(Cminus_in);
}
