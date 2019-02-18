/*******************************************************/
/*                     Cminus Parser                   */
/*                                                     */
/*******************************************************/

/*********************DEFINITIONS***********************/
%{
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <util/general.h>
#include <util/symtab.h>
#include <util/symtab_stack.h>
#include <util/dlink.h>
#include <util/string_utils.h>

#define SYMTABLE_SIZE 100
#define SYMTABLE_VALUE "value"

/*********************EXTERNAL DECLARATIONS***********************/

EXTERN(void,Cminus_error,(const char*));

EXTERN(int,Cminus_lex,(void));

char *fileName;

extern int Cminus_lineno;

extern FILE *Cminus_in;

static SymTable VariableTable;

static int intPow(int a, int e)
{
    int result = a;
    if (e < 1)
        return 0;
    if (e == 0)
        return 1;
    for (; e > 1; e -= 1)
        result *= a;
    return result;
}

%}

%name-prefix "Cminus_"
%defines

%start Program

%union {
    int intValue;
    int* intPointer;
    char* stringValue;
    DLinkList* linkedList;
}

%token AND
%token ELSE
%token EXIT
%token FOR
%token IF
%token INTEGER
%token NOT
%token OR
%token READ
%token GETC
%token WHILE
%token WRITE
%token PRINT
%token LEN_CALL
%token LBRACE
%token RBRACE
%token LE
%token LT
%token GE
%token GT
%token EQ
%token NE
%token ASSIGN
%token COMMA
%token SEMICOLON
%token LBRACKET
%token RBRACKET
%token LPAREN
%token RPAREN
%token PLUS
%token TIMES
%token IDENTIFIER
%token DIVIDE
%token RETURN
%token STRING
%token INTCON
%token MINUS
%token MODULO
%token EXPONENT
%token BAND
%token BOR
%token BNOT
%token BXOR
%token LSHIFT
%token RSHIFT
%token MULT_ASSIGN
%token DIV_ASSIGN
%token ADD_ASSIGN
%token SUB_ASSIGN
%token MOD_ASSIGN
%token BXOR_ASSIGN
%token BAND_ASSIGN
%token BOR_ASSIGN
%token EXPNT_ASSIGN
%token LAND_ASSIGN
%token LOR_ASSIGN
%token LSHIFT_ASSIGN
%token RSHIFT_ASSIGN

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

%type <stringValue> STRING StringConstant
%type <stringValue> IDENTIFIER
%type <intValue> Assignment
%type <intValue> INTCON Constant
%type <intValue> Factor BnotExpr ExpntExpr MulExpr AddExpr ShiftExpr SimpleExpr LogicExpr Expr
%type <intValue> IAssignExpr AssignExpr
%type <intPointer> Variable
%type <linkedList> ExprList

%destructor { if($$ != NULL) free($$); } StringConstant

/***********************PRODUCTIONS****************************/
%%
Program           : Procedures
                  | DeclList Procedures
                  ;

Procedures        : ProcedureDecl Procedures
                  |
                  ;

ProcedureDecl     : ProcedureHead ProcedureBody
                  ;

ProcedureHead     : FunctionDecl DeclList
                  | FunctionDecl
                  ;

FunctionDecl      : Type IDENTIFIER LPAREN RPAREN LBRACE
                  ;

ProcedureBody     : StatementList RBRACE
                  ;


DeclList          : Type IdentifierList  SEMICOLON
                  | DeclList Type IdentifierList SEMICOLON
                  ;


IdentifierList    : VarDecl
                  | IdentifierList COMMA VarDecl
                  ;

VarDecl           : IDENTIFIER                             { SymPutField(VariableTable, $1, SYMTABLE_VALUE, (Generic) malloc(sizeof(int))); }
                  | IDENTIFIER LBRACKET INTCON RBRACKET    { SymPutField(VariableTable, $1, SYMTABLE_VALUE, (Generic) malloc(sizeof(int) * $3)); }
                  | IDENTIFIER ASSIGN LogicExpr
                  {
                      int* temp = malloc(sizeof(int));
                      *temp = $3;
                      SymPutField(VariableTable, $1, SYMTABLE_VALUE, (Generic) temp);
                  }
                  | IDENTIFIER LBRACKET INTCON RBRACKET ASSIGN LogicExpr[Value]
                  {
                      int* temp = malloc(sizeof(int) * $3);
                      for(int i = 0; i < $3; i += 1)
                          temp[i] = $Value;
                      SymPutField(VariableTable, $1, SYMTABLE_VALUE, (Generic) temp);
                  }
                  | IDENTIFIER LBRACKET INTCON RBRACKET ASSIGN LBRACE ExprList[List] RBRACE
                  {
                      int* temp = malloc(sizeof(int) * $3);
                      DLinkNode *current = dlinkHead($List);
                      for(int i = 0; i < $3 && current != NULLNODE; i += 1, current = dlinkNext(current))
                          temp[i] = *((int*) dlinkNodeAtom(current));
                      dlinkFreeNodesAndAtoms($List);
                      dlinkListFree($List);
                      SymPutField(VariableTable, $1, SYMTABLE_VALUE, (Generic) temp);
                  }
                  | IDENTIFIER LBRACKET INTCON RBRACKET ASSIGN StringConstant[Value]
                  {
                      int* temp = malloc(sizeof(int) * $3);
                      for(int i = 0; i < $3; i += 1)
                          temp[i] = $Value[i];
                      SymPutField(VariableTable, $1, SYMTABLE_VALUE, (Generic) temp);
                  }
                  | IDENTIFIER LBRACKET RBRACKET ASSIGN StringConstant[Value]
                  {
                      int length = strlen($Value);
                      int* temp = malloc(sizeof(int) * (length + 1));
                      for(; length >= 0; length -= 1)
                          temp[length] = $Value[length];
                      SymPutField(VariableTable, $1, SYMTABLE_VALUE, (Generic) temp);
                  }
                  ;

Type              : INTEGER
                  ;

Statement         : Assignment
                  | IfStatement
                  | WhileStatement
                  | IOStatement
                  | ReturnStatement
                  | ExitStatement
                  | CompoundStatement
                  ;

Assignment        : AssignExpr SEMICOLON
                  ;

IfStatement       : IF TestAndThen ELSE CompoundStatement
                  | IF TestAndThen
                  ;


TestAndThen       : Test CompoundStatement
                  ;

Test              : LPAREN Expr RPAREN
                  ;

WhileStatement    : WhileToken WhileExpr Statement
                  ;

WhileExpr         : LPAREN Expr RPAREN
                  ;

WhileToken        : WHILE
                  ;


IOStatement       : READ LPAREN Variable RPAREN SEMICOLON        { scanf("%d", $3); }
                  | GETC LPAREN Variable RPAREN SEMICOLON        { *$3 = getchar(); }
                  | WRITE LPAREN Expr RPAREN SEMICOLON           { printf("%d\n", $3); }
                  | WRITE LPAREN StringConstant RPAREN SEMICOLON { printf("%s\n", $3); }
                  | PRINT LPAREN Expr RPAREN SEMICOLON           { printf("%c", $3); }
                  | PRINT LPAREN Expr COMMA Variable[Str] RPAREN SEMICOLON
                  {
                      for(int i = 0; i < $3; i += 1)
                          printf("%c", $Str[i]);
                  }
                  ;

ReturnStatement   : RETURN Expr SEMICOLON
                  ;

ExitStatement     : EXIT SEMICOLON
                  ;

CompoundStatement : LBRACE StatementList RBRACE
                  ;

StatementList     : Statement
                  | StatementList Statement
                  ;

Expr              : IAssignExpr
                  ;

AssignExpr        : Variable ASSIGN IAssignExpr           { $$ = *$1 = $3; }
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

IAssignExpr       : LogicExpr
                  | AssignExpr
                  ;

ExprList          : LogicExpr
                  {
                      $$ = dlinkListAlloc(NULL);
                      int* temp = malloc(sizeof(int));
                      *temp = $1;
                      dlinkAppend($$, dlinkNodeAlloc((Generic) temp));
                  }
                  | ExprList COMMA LogicExpr
                  {
                      int* temp = malloc(sizeof(int));
                      *temp = $3;
                      dlinkAppend($1, dlinkNodeAlloc((Generic) temp));
                      $$ = $1;
                  }
                  ;

LogicExpr         : SimpleExpr
                  | LogicExpr OR SimpleExpr     { $$ = $1 || $3; }
                  | LogicExpr AND SimpleExpr    { $$ = $1 && $3; }
                  | NOT LogicExpr               { $$ = !$2; }
                  ;

SimpleExpr        : ShiftExpr
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

ShiftExpr         : AddExpr
                  | ShiftExpr RSHIFT AddExpr    { $$ = $1 >> $3; }
                  | ShiftExpr LSHIFT AddExpr    { $$ = $1 << $3; }
                  ;

AddExpr           : MulExpr
                  | AddExpr PLUS MulExpr        { $$ = $1 + $3; }
                  | AddExpr MINUS MulExpr       { $$ = $1 - $3; }
                  ;

MulExpr           : ExpntExpr
                  | MulExpr TIMES ExpntExpr     { $$ = $1 * $3; }
                  | MulExpr DIVIDE ExpntExpr    { $$ = $1 / $3; }
                  | MulExpr MODULO ExpntExpr    { $$ = $1 % $3; }
                  ;

ExpntExpr         : BnotExpr
                  | ExpntExpr EXPONENT BnotExpr { $$ = intPow($1, $3); }
                  ;

BnotExpr          : Factor
                  | BNOT BnotExpr               { $$ = ~$2; }
                  ;

Factor            : Constant
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

Variable          : IDENTIFIER                        { $$ = (int*) SymGetField(VariableTable, $1, SYMTABLE_VALUE); }
                  | IDENTIFIER LBRACKET Expr RBRACKET { $$ = &(((int*) SymGetField(VariableTable, $1, SYMTABLE_VALUE))[$3]); }
                  ;

StringConstant    : STRING
                  ;

Constant          : INTCON
                  ;

%%


/********************C ROUTINES *********************************/

void Cminus_error(const char *s)
{
    fprintf(stderr,"%s: line %d: %s\n",fileName,Cminus_lineno,s);
}

int Cminus_wrap()
{
    return 1;
}

static void initialize(char* inputFileName)
{
    Cminus_in = fopen(inputFileName,"r");
        if (Cminus_in == NULL) {
          fprintf(stderr,"Error: Could not open file %s\n",inputFileName);
          exit(-1);
        }

    char* dotChar = rindex(inputFileName,'.');
    int endIndex = strlen(inputFileName) - strlen(dotChar);
    char *outputFileName = nssave(2,substr(inputFileName,0,endIndex),".s");
    stdout = freopen(outputFileName,"w",stdout);
        if (stdout == NULL) {
          fprintf(stderr,"Error: Could not open file %s\n",outputFileName);
          exit(-1);
        }
}

static void finalize()
{
    fclose(Cminus_in);
    fclose(stdout);
}

void safeFree(void* id)
{
    if(id != NULL)
        free(id);
}

int main(int argc, char** argv)
{
    fileName = argv[1];
    initialize(fileName);
    VariableTable = SymInit(SYMTABLE_SIZE);
    SymInitField(VariableTable, SYMTABLE_VALUE, NULL, safeFree);
    Cminus_parse();
    SymKill(VariableTable);
    finalize();
    return 0;
}
/******************END OF C ROUTINES**********************/
