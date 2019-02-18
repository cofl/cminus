WS     [ \t]
HEX    [a-fA-F0-9]
String \"[^\\\"]*(\\([\\\"abfnrtv'?0]|[xX][a-fA-F0-9][a-fA-F0-9])[^\\\"]*)*\"

%{

/* This scanner approximates the Void specification.
 *
 * Major differences:
 *
 *  (1) the printed spec is wrong in about sixteen ways in its definition
 *    of numeric constants.  The scanner recognizes a fairly broad class
 *     of things as constants - it allows both 0 and 1.0 as constants, where
 *    the document would not.
 *
 *      Feel free to accept the things that the scanner accepts as constants.
 *    The bug is in the original spec document.
 *
 *    Note: the scanner classifies all kinds of constants as "constant".
 *    It does not make a distinction between character, integer, and floating
 *    point constants.  I think of that distinction as something to look for
 *    in the type checker lab, not something that the grammar should enforce.
 *
 *
 * To hook this thing up to your parser, you will need to take the include
 * file of grammar terminals and relate them to the constants defined below.
 * You can either edit the flex spec or edit the constant defintions.  (Of
 * course, you should do that on your own copy!)
 *
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/symtab.h>
#include <util/symtab_stack.h>
#include <util/string_utils.h>
#include <util/dlink.h>
#include "CminusParser.h"

static int hc2i(char c)
{
    if(c >= 'a')
        return c - 'a' + 10;
    if(c >= 'A')
        return c - 'A' + 10;
    return c - '0';
}

static int emitString(const char* src)
{
    int i = 0, j = 0;
    char* temp = ssave(src + 1);
    while(i < Cminus_leng && temp[i] != '"')
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
                    fprintf(stderr, "Scanner: Unrecognized escaped character '%c' on line %d\n", temp[i], Cminus_lineno - 1);
                    break;
            }
        }
        i += 1;
        j += 1;
    }
    temp[j] = '\0';
    Cminus_lval.stringValue = temp;
    return STRING;
}

static int emitIdentifier(const char* src)
{
    Cminus_lval.stringValue = ssave(src);
    return IDENTIFIER;
}

static enum IntegerLiteralType
{
    Literal_Base10,
    Literal_Base16
};

static int emitInteger(enum IntegerLiteralType type, const char* src)
{
    switch(type)
    {
        case Literal_Base10:
            Cminus_lval = atoi(src);
            break;
        case Literal_Base16:
            sscanf(src, "%x", &(Cminus_lval.intValue));
            break;
    }
    return INTCON;
}

%}

%option bison-bridge
%option yylineno
%option prefix="Cminus_"

%x BLOCK_COMMENT
%x LINE_COMMENT
%%
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

    /* Symbol Constants */
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

    /* Strings */
{String} return emitString(Cminus_text);

    /* Identifiers / Literals */
[a-zA-Z_][a-zA-Z0-9_]* return emitIdentifier(Cminus_text);
0[xX][0-9a-fA-F]+      return emitInteger(Literal_Base16, Cminus_text);
[0-9]+                 return emitInteger(Literal_Base10, Cminus_text);

    /* Whitespace */
{WS}
"\n"

    /* Comments, with some help from https://stackoverflow.com/a/2130124 */
"/*"                BEGIN(BLOCK_COMMENT);
<BLOCK_COMMENT>"*/" BEGIN(INITIAL);
<BLOCK_COMMENT>"\n"
<BLOCK_COMMENT>.

"//"                BEGIN(LINE_COMMENT);
<LINE_COMMENT>"\n"  BEGIN(INITIAL);
<LINE_COMMENT>.

    /* fail on the rest */
. fprintf(stderr, "Scanner: lexical error '%s' on line %d.\n", Cminus_text, Cminus_lineno - 1);
%%
