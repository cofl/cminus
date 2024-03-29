%language "c++"
%code requires {
    namespace Cminus
    {
        class Driver;
        class Scanner;
    }
#include <iostream>
#include "ast/AST.hpp"

using namespace Cminus::Structures;
using namespace Cminus::AST;
}

%{
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "CminusDriver.hpp"
#include "CminusScanner.hpp"

Cminus::parser::symbol_type yylex(Cminus::Driver& driverInstance);
%}

%define api.namespace {Cminus}
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.trace
%define parse.error verbose

%defines
%locations
%param { Cminus::Driver& DriverInstance }
%parse-param { std::ostream& outputStream }

/* Reserved/Special */
%token
    END_OF_FILE 0 "end of file"
    ELSE
    EXIT
    IF
    INTEGER
    READ
    WHILE
    WRITE
    RETURN
    FOR
    BREAK
    CONTINUE
;

/* Literals */
%token <std::string>
    STRING
    IDENTIFIER
    ASM
;

%token <int> INTCON

/* Operators/Characters */
%token
    AND         "&&"
    NOT         "!"
    OR          "||"
    LBRACE      "{"
    RBRACE      "}"
    LE          "<="
    LT          "<"
    GE          ">="
    GT          ">"
    EQ          "=="
    NE          "!="
    ASSIGN      "="
    COMMA       ","
    SEMICOLON   ";"
    LBRACKET    "["
    RBRACKET    "]"
    LPAREN      "("
    RPAREN      ")"
    PLUS        "+"
    TIMES       "*"
    DIVIDE      "/"
    MINUS       "-"
    MODULO      "mod"
    EXPONENT    "**"
    BAND        "&"
    BOR         "|"
    BNOT        "~"
    BXOR        "^"
    LSHIFT      "<<"
    RSHIFT      ">>"
    MULT_ASSIGN "*="
    DIV_ASSIGN  "/="
    ADD_ASSIGN  "+="
    SUB_ASSIGN  "-="
    MOD_ASSIGN  "mod="
    BXOR_ASSIGN "^="
    BAND_ASSIGN "&="
    BOR_ASSIGN  "|="
    LAND_ASSIGN "&&="
    LOR_ASSIGN  "||="
    EXPNT_ASSIGN  "**="
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
%left TIMES DIVIDE MODULO
%left EXPONENT
%right ASSIGN MULT_ASSIGN DIV_ASSIGN ADD_ASSIGN SUB_ASSIGN MOD_ASSIGN BXOR_ASSIGN BAND_ASSIGN BOR_ASSIGN EXPNT_ASSIGN LAND_ASSIGN LOR_ASSIGN LSHIFT_ASSIGN RSHIFT_ASSIGN
%right NO_ELSE ELSE /* to fix shift-reduce conflict I introduced in if/else statement */

%type <int> Type
%type <StringLiteralASTNode*> StringConstant
%type <IntegerLiteralASTNode*> IntegerConstant
%type <ExpressionASTNode*> Variable Factor Expr BnotExpr ExpntExpr MulExpr AddExpr ShiftExpr SimpleExpr LogicExpr AssignExpr NegationExpr OptionalExpr
%type <ExpressionListASTNode*> ExprList
%type <ASTNode*> Statement ExitStatement ReturnStatement WhileStatement IfStatement IOStatement ForStatement
%type <StatementListASTNode*> StatementList
%type <SingleVariableDeclarationASTNode*> VarDecl
%type <VariableDeclarationASTNode*> VariableDecl IdentifierList
%type <FunctionDeclarationASTNode*> ProcedureDecl
%type <GlobalStatementListASTNode*> DeclList Procedures
%type <SimpleDeclarationASTNode*> FuncArg
%type <vector<SimpleDeclarationASTNode*>*> FuncArguments

%start Program
%%
Program             : Procedures
                    {
                        DriverInstance.Root = $1;
                    }
                    | DeclList Procedures
                    {
                        DriverInstance.Root = $1;
                        DriverInstance.Root->Members.insert(DriverInstance.Root->Members.end(), $2->Members.begin(), $2->Members.end());
                        $2->Members.clear();
                        delete $2;
                    }
                    |
                    {
                        DriverInstance.Root = nullptr;
                    }
                    ;

Procedures          : ProcedureDecl
                    {
                        $$ = new GlobalStatementListASTNode();
                        $$->Members.push_back($1);
                    }
                    | Procedures ProcedureDecl
                    {
                        $$ = $1;
                        $$->Members.push_back($2);
                    }
                    ;

ProcedureDecl       : Type IDENTIFIER "(" FuncArguments[Args] ")" "{" StatementList[Body] "}"
                    {
                        $$ = new FunctionDeclarationASTNode($2, $Body);
                        $$->Type = $1;
                        $$->Arguments = $Args;
                    }
                    | Type IDENTIFIER "(" ")" "{" StatementList[Body] "}"
                    {
                        $$ = new FunctionDeclarationASTNode($2, $Body);
                        $$->Type = $1;
                    }
                    ;

FuncArguments       : FuncArguments "," FuncArg { $$ = $1; $$->push_back($3); }
                    | FuncArg                   { $$ = new vector<SimpleDeclarationASTNode*>(); $$->push_back($1); }
                    ;

FuncArg             : Type IDENTIFIER           { $$ = new SimpleDeclarationASTNode($1, $2); }
                    ;

DeclList            : VariableDecl
                    {
                        $$ = new GlobalStatementListASTNode();
                        $$->Members.push_back($1);
                    }
                    | DeclList VariableDecl
                    {
                        $$ = $1;
                        $$->Members.push_back($2);
                    }
                    ;

VariableDecl        : Type IdentifierList ";"
                    {
                        $$ = $2;
                        $$->Type = $1;
                    }
                    ;

IdentifierList      : VarDecl
                    {
                        $$ = new VariableDeclarationASTNode();
                        $$->Members.push_back($1);
                    }
                    | IdentifierList "," VarDecl
                    {
                        $$ = $1;
                        $$->Members.push_back($3);
                    }
                    ;

VarDecl             : IDENTIFIER                         { $$ = new SingleVariableDeclarationASTNode($1, 1); }
                    | IDENTIFIER "[" IntegerConstant "]" { $$ = new SingleVariableDeclarationASTNode($1, $3->Value); }
                    | IDENTIFIER "=" LogicExpr           { $$ = new SingleVariableDeclarationASTNode($1, 1, $3); }
                    | IDENTIFIER "[" IntegerConstant "]" "=" Expr[Value]
                    {
                        $$ = new SingleVariableDeclarationASTNode($1, $3->Value, $Value);
                    }
                    | IDENTIFIER "[" IntegerConstant "]" "=" "{" ExprList[List] "}"
                    {
                        $$ = new SingleVariableDeclarationASTNode($1, $3->Value, $List);
                    }
                    ;

Type                : INTEGER               { $$ = DriverInstance.GetTypeID("int"); }
                    ;

Statement           : "{" StatementList "}" { $$ = $2; }
                    | IfStatement           { $$ = $1; }
                    | WhileStatement        { $$ = $1; }
                    | ForStatement          { $$ = $1; }
                    | IOStatement           { $$ = $1; }
                    | ReturnStatement       { $$ = $1; }
                    | ExitStatement         { $$ = $1; }
                    | Expr ";"              { $$ = $1; }
                    | VariableDecl          { $$ = $1; }
                    | BREAK ";"             { $$ = new BreakStatementASTNode(); }
                    | CONTINUE ";"          { $$ = new ContinueStatementASTNode(); }
                    | ";"                   { $$ = new NopStatementASTNode(); }
                    | ASM                   { $$ = new ASMASTNode($1); }
                    ;

IfStatement         : IF "(" Expr[Test] ")" Statement[IfTrueBody] %prec NO_ELSE
                    {
                        $$ = new IfStatementASTNode($Test, $IfTrueBody);
                    }
                    | IF "(" Expr[Test] ")" Statement[IfTrueBody] ELSE Statement[IfFalseBody]
                    {
                        $$ = new IfStatementASTNode($Test, $IfTrueBody, $IfFalseBody);
                    }
                    ;

WhileStatement      : WHILE "(" Expr[Test] ")" Statement[Body] { $$ = new WhileStatementASTNode($Test, $Body); }
                    ;

ForStatement        : FOR "(" OptionalExpr[Initial] ";" OptionalExpr[Test] ";" OptionalExpr[Final] ")" Statement[Body]
                    {
                        $$ = new ForStatementASTNode($Initial, $Test, $Final, $Body);
                    }
                    ;

IOStatement         : READ "(" Variable ")" ";"         { $$ = new ReadCallASTNode($3);  }
                    | WRITE "(" Expr ")" ";"            { $$ = new WriteCallASTNode($3); }
                    | WRITE "(" StringConstant ")" ";"  { $$ = new WriteCallASTNode($3); }
                    ;

ReturnStatement     : RETURN OptionalExpr ";"             { $$ = new ReturnStatementASTNode($2); }
                    ;

ExitStatement       : EXIT ";"                    { $$ = new ExitStatementASTNode(); }
                    ;

StatementList       : Statement
                    {
                        $$ = new StatementListASTNode();
                        $$->Members.push_back($1);
                    }
                    | StatementList Statement
                    {
                        $$ = $1;
                        $$->Members.push_back($2);
                    }
                    ;

OptionalExpr        : Expr  { $$ = $1; }
                    |       { $$ = nullptr; }
                    ;

Expr                : LogicExpr                   { $$ = $1; }
                    | AssignExpr                  { $$ = $1; }
                    ;

AssignExpr          : Variable ASSIGN Expr        { $$ = new BinaryOperationASTNode(ASTOperationType::Assign, $1, $3); }
                    | Variable MULT_ASSIGN Expr   { $$ = new BinaryOperationASTNode(ASTOperationType::Assign, $1,
                                                            new BinaryOperationASTNode(ASTOperationType::Multiply, $1, $3)); }
                    | Variable DIV_ASSIGN Expr    { $$ = new BinaryOperationASTNode(ASTOperationType::Assign, $1,
                                                            new BinaryOperationASTNode(ASTOperationType::Divide, $1, $3)); }
                    | Variable ADD_ASSIGN Expr    { $$ = new BinaryOperationASTNode(ASTOperationType::Assign, $1,
                                                            new BinaryOperationASTNode(ASTOperationType::Add, $1, $3)); }
                    | Variable SUB_ASSIGN Expr    { $$ = new BinaryOperationASTNode(ASTOperationType::Assign, $1,
                                                            new BinaryOperationASTNode(ASTOperationType::Subtract, $1, $3)); }
                    | Variable MOD_ASSIGN Expr    { $$ = new BinaryOperationASTNode(ASTOperationType::Assign, $1,
                                                            new BinaryOperationASTNode(ASTOperationType::Modulo, $1, $3)); }
                    | Variable BXOR_ASSIGN Expr   { $$ = new BinaryOperationASTNode(ASTOperationType::Assign, $1,
                                                            new BinaryOperationASTNode(ASTOperationType::BinaryXor, $1, $3)); }
                    | Variable BAND_ASSIGN Expr   { $$ = new BinaryOperationASTNode(ASTOperationType::Assign, $1,
                                                            new BinaryOperationASTNode(ASTOperationType::BinaryAnd, $1, $3)); }
                    | Variable BOR_ASSIGN Expr    { $$ = new BinaryOperationASTNode(ASTOperationType::Assign, $1,
                                                            new BinaryOperationASTNode(ASTOperationType::BinaryOr, $1, $3)); }
                    | Variable EXPNT_ASSIGN Expr  { $$ = new BinaryOperationASTNode(ASTOperationType::Assign, $1,
                                                            new BinaryOperationASTNode(ASTOperationType::Exponentiation, $1, $3)); }
                    | Variable LAND_ASSIGN Expr   { $$ = new BinaryOperationASTNode(ASTOperationType::Assign, $1,
                                                            new BinaryOperationASTNode(ASTOperationType::LogicalAnd, $1, $3)); }
                    | Variable LOR_ASSIGN Expr    { $$ = new BinaryOperationASTNode(ASTOperationType::Assign, $1,
                                                            new BinaryOperationASTNode(ASTOperationType::LogicalOr, $1, $3)); }
                    | Variable LSHIFT_ASSIGN Expr { $$ = new BinaryOperationASTNode(ASTOperationType::Assign, $1,
                                                            new BinaryOperationASTNode(ASTOperationType::LeftShift, $1, $3)); }
                    | Variable RSHIFT_ASSIGN Expr { $$ = new BinaryOperationASTNode(ASTOperationType::Assign, $1,
                                                            new BinaryOperationASTNode(ASTOperationType::RightShift, $1, $3)); }
                    ;

ExprList            : LogicExpr
                    {
                        $$ = new ExpressionListASTNode();
                        $$->Members.push_back($1);
                    }
                    | ExprList "," LogicExpr
                    {
                        $$ = $1;
                        $$->Members.push_back($3);
                    }
                    ;

LogicExpr           : SimpleExpr                  { $$ = $1; }
                    | LogicExpr OR SimpleExpr     { $$ = new BinaryOperationASTNode(ASTOperationType::LogicalOr, $1, $3); }
                    | LogicExpr AND SimpleExpr    { $$ = new BinaryOperationASTNode(ASTOperationType::LogicalAnd, $1, $3); }
                    | NOT LogicExpr               { $$ = new UnaryOperationASTNode(ASTOperationType::LogicalNot, $2); }
                    ;

SimpleExpr          : ShiftExpr                   { $$ = $1; }
                    | SimpleExpr EQ ShiftExpr     { $$ = new BinaryOperationASTNode(ASTOperationType::TestEQ, $1, $3); }
                    | SimpleExpr NE ShiftExpr     { $$ = new BinaryOperationASTNode(ASTOperationType::TestNE, $1, $3); }
                    | SimpleExpr LE ShiftExpr     { $$ = new BinaryOperationASTNode(ASTOperationType::TestLE, $1, $3); }
                    | SimpleExpr LT ShiftExpr     { $$ = new BinaryOperationASTNode(ASTOperationType::TestLT, $1, $3); }
                    | SimpleExpr GE ShiftExpr     { $$ = new BinaryOperationASTNode(ASTOperationType::TestGE, $1, $3); }
                    | SimpleExpr GT ShiftExpr     { $$ = new BinaryOperationASTNode(ASTOperationType::TestGT, $1, $3); }
                    | SimpleExpr BAND ShiftExpr   { $$ = new BinaryOperationASTNode(ASTOperationType::BinaryAnd, $1, $3); }
                    | SimpleExpr BOR ShiftExpr    { $$ = new BinaryOperationASTNode(ASTOperationType::BinaryOr, $1, $3); }
                    | SimpleExpr BXOR ShiftExpr   { $$ = new BinaryOperationASTNode(ASTOperationType::BinaryXor, $1, $3); }
                    ;

ShiftExpr           : AddExpr                     { $$ = $1; }
                    | ShiftExpr RSHIFT AddExpr    { $$ = new BinaryOperationASTNode(ASTOperationType::RightShift, $1, $3); }
                    | ShiftExpr LSHIFT AddExpr    { $$ = new BinaryOperationASTNode(ASTOperationType::LeftShift, $1, $3); }
                    ;

AddExpr             : MulExpr                     { $$ = $1; }
                    | AddExpr PLUS MulExpr        { $$ = new BinaryOperationASTNode(ASTOperationType::Add, $1, $3); }
                    | AddExpr MINUS MulExpr       { $$ = new BinaryOperationASTNode(ASTOperationType::Subtract, $1, $3); }
                    ;

MulExpr             : ExpntExpr                   { $$ = $1; }
                    | MulExpr TIMES ExpntExpr     { $$ = new BinaryOperationASTNode(ASTOperationType::Multiply, $1, $3); }
                    | MulExpr DIVIDE ExpntExpr    { $$ = new BinaryOperationASTNode(ASTOperationType::Divide, $1, $3); }
                    | MulExpr MODULO ExpntExpr    { $$ = new BinaryOperationASTNode(ASTOperationType::Modulo, $1, $3); }
                    ;

ExpntExpr           : BnotExpr                    { $$ = $1; }
                    | ExpntExpr EXPONENT BnotExpr { $$ = new BinaryOperationASTNode(ASTOperationType::Exponentiation, $1, $3); }
                    ;

BnotExpr            : NegationExpr                { $$ = $1; }
                    | BNOT BnotExpr               { $$ = new UnaryOperationASTNode(ASTOperationType::BinaryNot, $2); }
                    ;

NegationExpr        : Factor                  { $$ = $1; }
                    | MINUS NegationExpr          { $$ = new UnaryOperationASTNode(ASTOperationType::Negation, $2); }
                    ;

Factor              : IntegerConstant             { $$ = $1; }
                    | Variable                    { $$ = $1; }
                    | "(" Expr ")"                { $$ = $2; }
                    | IDENTIFIER "(" ")"          { $$ = new FunctionCallASTNode($1); }
                    | IDENTIFIER "(" ExprList ")" { $$ = new FunctionCallASTNode($1, $3); }
                    ;

Variable            : IDENTIFIER                  { $$ = new VariableASTNode($1); }
                    | IDENTIFIER "[" Expr "]"     { $$ = new VariableASTNode($1, $3); }
                    ;

IntegerConstant     : INTCON                      { $$ = new IntegerLiteralASTNode(DriverInstance.GetTypeID("int"), $1); }
                    ;

StringConstant      : STRING                      { $$ = new StringLiteralASTNode(DriverInstance.GetTypeID("string"), $1); }
                    ;
%%

void Cminus::parser::error(const location_type& locationType, const std::string& msg)
{
    std::cerr << locationType << ": " << msg << "\n";
}

Cminus::parser::symbol_type yylex(Cminus::Driver& driverInstance)
{
    return driverInstance.NextSymbol();
}
