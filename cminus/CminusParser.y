%language "c++"

%{
#include <string>
#include <cstdlib>
#include <cstdio>
#include "CminusDriver.hh"
%}

%name-prefix "Cminus_"
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.trace
%define parse.error verbose

%defines
%locations
%param { CminusDriver& DriverInstance }

/* Reserved/Special */
%token
    END_OF_FILE 0 "end of file"
    ELSE
    EXIT
    FOR
    IF
    INTEGER
    READ
    WHILE
    WRITE
    RETURN
;

%token
    GETC
    PRINT
    LEN_CALL
;

/* Literals */
%token
    IDENTIFIER
    STRING
    INTCON
;

/* Operators/Characters */
%token
    AND     "&&"
    NOT     "!"
    OR      "||"
    LBRACE  "{"
    RBRACE  "}"
    LE      "<="
    LT      "<"
    GE      ">="
    GT      ">"
    EQ      "=="
    NE      "!="
    ASSIGN  "="
    COMMA   ","
    SEMICOLON   ";"
    LBRACKET    "["
    RBRACKET    "]"
    LPAREN  "("
    RPAREN  ")"
    PLUS    "+"
    TIMES   "*"
    DIVIDE  "/"
    MINUS   "-"
    MODULO  "mod"
    EXPONENT    "**"
    BAND    "&"
    BOR     "|"
    BNOT    "~"
    BXOR    "^"
    LSHIFT  "<<"
    RSHIFT  ">>"
    MULT_ASSIGN "*="
    DIV_ASSIGN  "/="
    ADD_ASSIGN  "+="
    SUB_ASSIGN  "-="
    MOD_ASSIGN  "mod="
    BXOR_ASSIGN "^="
    BAND_ASSIGN "&="
    BOR_ASSIGN  "|="
    EXPNT_ASSIGN "**="
    LAND_ASSIGN "&&="
    LOR_ASSIGN  "||="
    LSHIFT_ASSIGN "<<="
    RSHIFT_ASSIGN ">>="
;

%left OR
%left AND
%left NOT
%left BNOT
%left BAND BOR BXOR
%left LT LE GT GE NE EQ
%left PLUS MINUS
%left TIMES DIVDE MODULO
%left EXPONENT
%right ASSIGN MULT_ASSIGN DIV_ASSIGN ADD_ASSIGN SUB_ASSIGN MOD_ASSIGN BXOR_ASSIGN BAND_ASSIGN BOR_ASSIGN EXPNT_ASSIGN LAND_ASSIGN LOR_ASSIGN LSHIFT_ASSIGN RSHIFT_ASSIGN

%type <std::string> STRING
%type <std::string> IDENTIFIER
%type <int> Assignment
%type <int> INTCON
%type <int> Factor BnotExpr ExpntExpr MulExpr AddExpr ShiftExpr SimpleExpr LogicExpr Expr
%type <int> IAssignExpr AssignExpr
%type <int*> Variable
%type <std::vector<int>> ExprList

%start Program
%%
Program             : Procedures
                    | DeclList Procedures
                    ;

Procedures          : ProcedureDecl Procedures
                    |
                    ;

ProcedureDecl       : ProcedureHead ProcedureBody
                    ;

ProcedureHead       : FunctionDecl DeclList
                    | FunctionDecl
                    ;

FunctionDecl        : Type IDENTIFIER "(" ")" "{"
                    ;

ProcedureBody       : StatementList "}"
                    ;

DeclList            : Type IdentifierList ";"
                    | DeclList Type IdentifierList ";"
                    ;

IdentifierList      : VarDecl
                    | IdentifierList "," VarDecl
                    ;

VarDecl             : IDENTIFIER                { DriverInstance.Variables[$1] = malloc(sizeof(int)); }
                    | IDENTIFIER "[" INTCON "]" { DriverInstance.Variables[$1] = malloc(sizeof(int) * $3); }
                    | IDENTIFIER "=" LogicExpr
                    {
                        int* temp = malloc(sizeof(int));
                        *temp = $3;
                        DriverInstance.Variables[$1] = temp;
                    }
                    | IDENTIFIER "[" INTCON[Size] "]" "=" LogicExpr[Value]
                    {
                        int* temp = malloc(sizeof(int) * $Size);
                        for(int i = 0; i < $3; i += 1)
                            temp[i] = $Value;
                        DriverInstance.Variables[$1] = temp;
                    }
                    | IDENTIFIER "[" INTCON[Size] "]" "=" "{" ExprList[List] "}"
                    {
                        int listSize = $List.size();
                        if(listSize > $Size)
                            throw Cminus::parser::syntax_error(DriverInstance.Location, "Expression list is longer than declared size.");
                        int* temp = malloc(sizeof(int) * $Size);
                        for(int i = 0; i < listSize; i += 1)
                            temp[i] = $List[i];
                        DriverInstance.Variables[$1] = temp;
                    }
                    | IDENTIFIER "[" INTCON[Size] "]" "=" STRING[Value]
                    {
                        int listSize = $Value.size();
                        // allow for null at the end.
                        if(listSize + 1 <= $Size)
                            listSize += 1;
                        if(listSize > $Size)
                            throw Cminus::parser::syntax_error(DriverInstance.Location, "String is longer than declared size.");
                        int* temp = malloc(sizeof(int) * $Size);
                        for(int i = 0; i < listSize; i += 1)
                            temp[i] = $Value[i];
                        DriverInstance.Variables[$1] = temp;
                    }
                    | IDENTIFIER "[" "]" "=" STRING[Value]
                    {
                        int length = $Value.size();
                        int* temp = malloc(sizeof(int) * (length + 1))
                        for(; length >= 0; length -= 1)
                            temp[length] = $Value[length];
                        DriverInstance.Variables[$1] = temp;
                    }
                    ;

Type                : INTEGER
                    ;

Statement           : Assignment
                    | IfStatement
                    | WhileStatement
                    | IOStatement
                    | ReturnStatement
                    | ExitStatement
                    | CompoundStatement
                    ;

Assignment          : AssignExpr ";"
                    ;

IfStatement         : IF TestAndThen ELSE CompoundStatement
                    | IF TestAndThen
                    ;

TestAndThen         : Test CompoundStatement
                    ;

Test                : "(" Expr ")"
                    ;

WhileStatement      : WHILE WhileExpr Statement
                    ;

WhileExpr           : "(" Expr ")"
                    ;

IOStatement         : READ "(" Variable ")" ";"     { scanf("%d", $3); }
                    | GETC "(" Variable ")" ";"     { *$3 = getchar(); }
                    | WRITE "(" Expr ")" ";"        { printf("%d\n", $3); }
                    | WRITE "(" STRING ")" ";"      { printf("%s\n", $3); }
                    | PRINT "(" Expr ")" ";"        { printf("%c", $3); }
                    | PRINT "(" Expr "," Variable[Str] ")" ";"
                    {
                        for(int i = 0; i < $3; i += 1)
                            printf("%c", $Str[i]);
                    }
                    ;

ReturnStatement     : RETURN Expr ";"
                    ;

ExitStatement       : EXIT ";"
                    ;

CompoundStatement   : "{" StatementList "}"
                    ;

StatementList       : Statement
                    | StatementList Statement
                    ;

Expr                : IAssignExpr
                    ;

AssignExpr          : Variable ASSIGN IAssignExpr           { $$ = *$1 = $3; }
                    | Variable MULT_ASSIGN IAssignExpr      { $$ = *$1 = (*$1 * $3); }
                    | Variable DIV_ASSIGN IAssignExpr       { $$ = (*$1 = *$1 / $3); }
                    | Variable ADD_ASSIGN IAssignExpr       { $$ = (*$1 = *$1 + $3); }
                    | Variable SUB_ASSIGN IAssignExpr       { $$ = (*$1 = *$1 - $3); }
                    | Variable MOD_ASSIGN IAssignExpr       { $$ = (*$1 = *$1 % $3); }
                    | Variable BXOR_ASSIGN IAssignExpr      { $$ = (*$1 = *$1 ^ $3); }
                    | Variable BAND_ASSIGN IAssignExpr      { $$ = (*$1 = *$1 & $3); }
                    | Variable BOR_ASSIGN IAssignExpr       { $$ = (*$1 = *$1 | $3); }
                    | Variable EXPNT_ASSIGN IAssignExpr     { $$ = (*$1 = intPow(*$1, $3)); }
                    | Variable LAND_ASSIGN IAssignExpr      { $$ = (*$1 = *$1 && $3); }
                    | Variable LOR_ASSIGN IAssignExpr       { $$ = (*$1 = *$1 || $3); }
                    | Variable LSHIFT_ASSIGN IAssignExpr    { $$ = (*$1 = *$1 << $3); }
                    | Variable RSHIFT_ASSIGN IAssignExpr    { $$ = (*$1 = *$1 >> $3); }
                    ;

IAssignExpr         : LogicExpr
                    | AssignExpr
                    ;

ExprList            : LogicExpr
                    {
                        $$ = new std::vector<int>();
                        $$.push_back($1);
                    }
                    | ExprList "," LogicExpr
                    {
                        $$ = $1;
                        $$.push_back($3);
                    }
                    ;

LogicExpr           : SimpleExpr
                    | LogicExpr OR SimpleExpr     { $$ = $1 || $3; }
                    | LogicExpr AND SimpleExpr    { $$ = $1 && $3; }
                    | NOT LogicExpr               { $$ = !$2; }
                    ;

SimpleExpr          : ShiftExpr
                    | SimpleExpr EQ ShiftExpr     { $$ = $1 == $3; }
                    | SimpleExpr NE ShiftExpr     { $$ = $1 != $3; }
                    | SimpleExpr LE ShiftExpr     { $$ = $1 <= $3; }
                    | SimpleExpr LT ShiftExpr     { $$ = $1 < $3; }
                    | SimpleExpr GE ShiftExpr     { $$ = $1 >= $3; }
                    | SimpleExpr GT ShiftExpr     { $$ = $1 > $3; }
                    | SimpleExpr BAND ShiftExpr   { $$ = $1 & $3; }
                    | SimpleExpr BOR ShiftExpr    { $$ = $1 | $3; }
                    | SimpleExpr BXOR ShiftExpr   { $$ = $1 ^ $3; }
                    ;

ShiftExpr           : AddExpr
                    | ShiftExpr RSHIFT AddExpr    { $$ = $1 >> $3; }
                    | ShiftExpr LSHIFT AddExpr    { $$ = $1 << $3; }
                    ;

AddExpr             : MulExpr
                    | AddExpr PLUS MulExpr        { $$ = $1 + $3; }
                    | AddExpr MINUS MulExpr       { $$ = $1 - $3; }
                    ;

MulExpr             : ExpntExpr
                    | MulExpr TIMES ExpntExpr     { $$ = $1 * $3; }
                    | MulExpr DIVIDE ExpntExpr    { $$ = $1 / $3; }
                    | MulExpr MODULO ExpntExpr    { $$ = $1 % $3; }
                    ;

ExpntExpr           : BnotExpr
                    | ExpntExpr EXPONENT BnotExpr { $$ = intPow($1, $3); }
                    ;

BnotExpr            : Factor
                    | BNOT BnotExpr               { $$ = ~$2; }
                    ;

Factor              : INTCON
                    | Variable                    { $$ = *$1; }
                    | LEN_CALL LPAREN Variable RPAREN
                    {
                        int safety = 4096;
                        $$ = 0;
                        while($3[$$] != 0 && safety > 0)
                            $$ += 1;
                    }
                    | IDENTIFIER LPAREN RPAREN    { $$ = 0; /* safety case */ }
                    | LPAREN Expr RPAREN          { $$ = $2; }
                    | MINUS Factor                { $$ = -$2; }
                    ;

Variable            : IDENTIFIER                  { $$ = (int*) (DriverInstance.Variables[$1]); }
                    | IDENTIFIER "[" Expr "]"     { $$ = &(((int*) (DriverInstance.Variable[$1]))[$3]); }
                    ;

%%

void Cminus::parser::error(const location_type& locationType, const std::string& msg)
{
    std::cerr << locationType << ": " << msg << "\n";
}

static void init(std::string inputFileName)
{
    if(inputFileName.empty() || inputFileName == "-")
        return;
    auto outputFile = inputFileName.substr(0, inputFileName.rfind('.')) + ".s";
    stdout = freopen(,"w",stdout);
    if (stdout == NULL)
    {
        cerr << "Error: Could not open file " << outputFile << "\n";
        exit(-1);
    }
}

int main(int argc, char** argv)
{
    auto inputFileName = argc > 1 ? argv[1] : "";
    CminusDriver driver;
    init(inputFileName)
    return driver.Parse(inputFileName);
}
