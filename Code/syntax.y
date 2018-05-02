%{
    #include <stdio.h>
    #include "parse.h"
    extern int yylex(void);
    extern void yyerror(char* msg);
    extern void myerror(char* msg);
    extern void semerror(char* msg);
    Node *root = NULL;

%}

%union {
    struct TreeNode* node;
}

/* declared tokens */

%token <node> INT FLOAT ID
%token <node> SEMI COMMA ASSIGNOP
%token <node> RELOP
%token <node> PLUS MINUS STAR DIV
%token <node> AND OR DOT NOT
%token <node> TYPE
%token <node> LP RP LB RB LC RC
%token <node> STRUCT RETURN IF ELSE WHILE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

/* declared non-terminals */

%type <node> Program ExtDefList ExtDef ExtDecList
%type <node> Specifier StructSpecifier OptTag Tag
%type <node> VarDec FunDec VarList ParamDec
%type <node> CompSt StmtList Stmt
%type <node> DefList Def DecList Dec
%type <node> Exp Args

%%
/* High-level Definitions */

Program : ExtDefList { $$ = createNode("Program", ""); insertNode(1, $$, $1); root = $$; }
  ;
ExtDefList : ExtDef ExtDefList { $$ = createNode("ExtDefList", ""); insertNode(2, $$, $1, $2); }
  | { $$ = NULL; } /* empty */
  ;
ExtDef : Specifier ExtDecList SEMI { $$ = createNode("ExtDef", ""); insertNode(3, $$, $1, $2, $3); }
  | Specifier SEMI { $$ = createNode("ExtDef", ""); insertNode(2, $$, $1, $2); }
  | Specifier error SEMI { myerror("Illegal definitions of global variables, struct or functions"); }
  | Specifier FunDec CompSt { $$ = createNode("ExtDef", ""); insertNode(3, $$, $1, $2, $3); }
  ;

ExtDecList : VarDec { $$ = createNode("ExtDecList", ""); insertNode(1, $$, $1); }
  | VarDec COMMA ExtDecList { $$ = createNode("ExtDecList", ""); insertNode(3, $$, $1, $2, $3); }
  ;


/* Specifiers */

Specifier : TYPE { $$ = createNode("Specifier", ""); insertNode(1, $$, $1); }
  | StructSpecifier { $$ = createNode("Specifier", ""); insertNode(1, $$, $1); }
  ;

StructSpecifier : STRUCT OptTag LC DefList RC { $$ = createNode("StructSpecifier", ""); insertNode(5, $$, $1, $2, $3, $4, $5); }
  | STRUCT Tag { $$ = createNode("StructSpecifier", ""); insertNode(2, $$, $1, $2); }
  ;

OptTag : ID { $$ = createNode("OptTag", ""); insertNode(1, $$, $1); }
  | { $$ = NULL; } /* empty */
  ;

Tag : ID { $$ = createNode("Tag", ""); insertNode(1, $$, $1); }
  ;


/* Declarators */

VarDec : ID { $$ = createNode("VarDec", ""); insertNode(1, $$, $1); }
  | TYPE { myerror("Unvalid name of variable"); }
  | VarDec LB INT RB { $$ = createNode("VarDec", ""); insertNode(4, $$, $1, $2, $3, $4); }
  ;

FunDec : ID LP VarList RP { $$ = createNode("FunDec", ""); insertNode(4, $$, $1, $2, $3, $4); }
  | ID LP RP { $$ = createNode("FunDec", ""); insertNode(3, $$, $1, $2, $3); }
  ;

VarList : ParamDec COMMA VarList { $$ = createNode("VarList", ""); insertNode(3, $$, $1, $2, $3); }
  | ParamDec { $$ = createNode("VarList", ""); insertNode(1, $$, $1); }
  ;

ParamDec : Specifier VarDec { $$ = createNode("ParamDec", ""); insertNode(2, $$, $1, $2); }
  ;


/* Statements */

CompSt : LC DefList StmtList RC { $$ = createNode("CompSt", ""); insertNode(4, $$, $1, $2, $3, $4); }
  | LC error RC { myerror("Unvalid statement block"); }
  ;

StmtList : Stmt StmtList { $$ = createNode("StmtList", ""); insertNode(2, $$, $1, $2); }
  | { $$ = NULL; } /* empty */
  ;

Stmt : Exp SEMI { $$ = createNode("Stmt", ""); insertNode(2, $$, $1, $2); }
  | Exp error SEMI { myerror("Unvalid statement"); }
  | CompSt { $$ = createNode("Stmt", ""); insertNode(1, $$, $1); }
  | RETURN Exp SEMI { $$ = createNode("Stmt", ""); insertNode(3, $$, $1, $2, $3); }
  | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE { $$ = createNode("Stmt", ""); insertNode(5, $$, $1, $2, $3, $4, $5); }
  | IF LP Exp RP Stmt ELSE Stmt { $$ = createNode("Stmt", ""); insertNode(7, $$, $1, $2, $3, $4, $5, $6, $7); }
  | IF LP Exp RP Exp error ELSE Stmt { myerror("Missing \";\""); }
  | WHILE LP Exp RP Stmt { $$ = createNode("Stmt", ""); insertNode(5, $$, $1, $2, $3, $4, $5); }
  ;


/* Local Definitions */

DefList : Def DefList { $$ = createNode("DefList", ""); insertNode(2, $$, $1, $2); }
  | { $$ = NULL; } /* empty */
  ;

Def : Specifier DecList SEMI { $$ = createNode("Def", ""); insertNode(3, $$, $1, $2, $3); }
  | error DecList SEMI { myerror("Unvalid identifier"); }
  ;

DecList : Dec { $$ = createNode("DecList", ""); insertNode(1, $$, $1); }
  | Dec COMMA DecList { $$ = createNode("DecList", ""); insertNode(3, $$, $1, $2, $3); }
  ;

Dec : VarDec { $$ = createNode("Dec", ""); insertNode(1, $$, $1); }
  | VarDec ASSIGNOP Exp { $$ = createNode("Dec", ""); insertNode(3, $$, $1, $2, $3); }
  ;


/* Expressions */

Exp : Exp ASSIGNOP Exp { $$ = createNode("Exp", ""); insertNode(3, $$, $1, $2, $3); }
  | Exp AND Exp { $$ = createNode("Exp", ""); insertNode(3, $$, $1, $2, $3); }
  | Exp OR Exp { $$ = createNode("Exp", ""); insertNode(3, $$, $1, $2, $3); }
  | Exp RELOP Exp { $$ = createNode("Exp", ""); insertNode(3, $$, $1, $2, $3); }
  | Exp PLUS Exp { $$ = createNode("Exp", ""); insertNode(3, $$, $1, $2, $3); }
  | Exp MINUS Exp { $$ = createNode("Exp", ""); insertNode(3, $$, $1, $2, $3); }
  | Exp STAR Exp { $$ = createNode("Exp", ""); insertNode(3, $$, $1, $2, $3); }
  | Exp DIV Exp { $$ = createNode("Exp", ""); insertNode(3, $$, $1, $2, $3); }
  | LP Exp RP { $$ = createNode("Exp", ""); insertNode(3, $$, $1, $2, $3); }
  | LP error RP { myerror("Missing \")\""); }
  | MINUS Exp { $$ = createNode("Exp", ""); insertNode(2, $$, $1, $2); }
  | NOT Exp { $$ = createNode("Exp", ""); insertNode(2, $$, $1, $2); }
  | ID LP Args RP { $$ = createNode("Exp", ""); insertNode(4, $$, $1, $2, $3, $4); }
  | ID LP RP { $$ = createNode("Exp", ""); insertNode(3, $$, $1, $2, $3); }
  | Exp LB Exp RB { $$ = createNode("Exp", ""); insertNode(4, $$, $1, $2, $3, $4); }
  | Exp LB error RB { myerror("Missing \"]\""); }
  | Exp LB Exp error { myerror("Missing \"]\""); }
  | Exp DOT ID { $$ = createNode("Exp", ""); insertNode(3, $$, $1, $2, $3); }
  | ID { $$ = createNode("Exp", ""); insertNode(1, $$, $1); }
  | INT { $$ = createNode("Exp", ""); insertNode(1, $$, $1); }
  | FLOAT { $$ = createNode("Exp", ""); insertNode(1, $$, $1); }
  ;

Args : Exp COMMA Args { $$ = createNode("Args", ""); insertNode(3, $$, $1, $2, $3); }
  | Exp { $$ = createNode("Args", ""); insertNode(1, $$, $1); }
  ;

%%
#include "lex.yy.c"

void yyerror(char* msg) {

}

void myerror(char* msg) {
    errorNum++;
    PrintErrorMsg("B", msg);
}

void semerror(char* msg) {
    printf("%s\n", msg);
}
