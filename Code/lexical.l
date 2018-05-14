%option yylineno
%{
    #include "syntax.tab.h"
    #include "parse.h"
%}
DIGIT [0-9]
DIGITS {DIGIT}+
DEC (0|([1-9][0-9]*))
OCT 0[1-7][0-7]*
OCT_ERR 0{DIGITS}
HEX 0[Xx][0-9A-Fa-f]+
HEX_ERR 0[Xx][0-9A-Za-z_]*
INT {DEC}|{OCT}|{HEX}
FLOAT {DEC}\.{DIGITS}
FLOAT_ERR ({DIGITS}[0-9A-Za-z_]*\.[0-9A-Za-z_\.+-]*)|(\.{DIGITS}[0-9A-Za-z_+-]*)
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
DOT "."
NOT "!"
TYPE int|float
ID [A-Za-z_][0-9A-Za-z_]*
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
ENTER \n
WS [ \t\r\n]+
COMMENTS \/\/
LCOMMENTS \/\*
RCOMMENTS \*\/
DOUPLUS "++"
PLUSASS "+="
MINUSASS "-="
UNHANDLE .
%%
{SEMI}        { CreateNode("SEMI");                  return SEMI; }
{COMMA}       { CreateNode("COMMA");                return COMMA; }
{ASSIGNOP}    { CreateNode("ASSIGNOP");          return ASSIGNOP; }
{RELOP}       { CreateNode("RELOP");                return RELOP; }
{PLUS}        { CreateNode("PLUS");                  return PLUS; }
{MINUS}       { CreateNode("MINUS");                return MINUS; }
{STAR}        { CreateNode("STAR");                  return STAR; }
{DIV}         { CreateNode("DIV");                    return DIV; }
{AND}         { CreateNode("AND");                    return AND; }
{OR}          { CreateNode("OR");                      return OR; }
{DOT}         { CreateNode("DOT");                    return DOT; }
{NOT}         { CreateNode("NOT");                    return NOT; }
{TYPE}        { CreateNode("TYPE");                  return TYPE; }

{LP}          { CreateNode("LP");                      return LP; }
{RP}          { CreateNode("RP");                      return RP; }
{LB}          { CreateNode("LB");                      return LB; }
{RB}          { CreateNode("RB");                      return RB; }
{LC}          { CreateNode("LC");                      return LC; }
{RC}          { CreateNode("RC");                      return RC; }

{STRUCT}      { CreateNode("STRUCT");              return STRUCT; }
{RETURN}      { CreateNode("RETURN");              return RETURN; }
{IF}          { CreateNode("IF");                      return IF; }
{ELSE}        { CreateNode("ELSE");                  return ELSE; }
{WHILE}       { CreateNode("WHILE");                return WHILE; }

{FLOAT}       { CreateNode("FLOAT");                return FLOAT; }

{OCT}         { oct2Dec(yytext);  CreateNode("INT");  return INT; }
{DEC}         {                   CreateNode("INT");  return INT; }
{HEX}         { hex2Dec(yytext);  CreateNode("INT");  return INT; }

{ID}          { CreateNode("ID");                      return ID; }

{WS}          { ; }

{FLOAT_ERR}   { errorNum++; errorMsg("Illegal floating point number"); PrintErrorMsg("A", errormsg); return INT; }
{OCT_ERR}     { errorNum++; errorMsg("Illegal octal number"); PrintErrorMsg("A", errormsg); return INT; }
{HEX_ERR}     { errorNum++; errorMsg("Illegal hexadecimal number"); PrintErrorMsg("A", errormsg); return INT; }
{COMMENTS}    { errorNum++; errorMsg("Mysterious character"); PrintErrorMsg("A", errormsg); }
{LCOMMENTS}   { errorNum++; errorMsg("Mysterious character"); PrintErrorMsg("A", errormsg); }
{RCOMMENTS}   { errorNum++; errorMsg("Mysterious character"); PrintErrorMsg("A", errormsg); }
{DOUPLUS}     { errorNum++; errorMsg("Illegal characters"); PrintErrorMsg("A", errormsg); }
{PLUSASS}     { errorNum++; errorMsg("Illegal characters"); PrintErrorMsg("A", errormsg); }
{MINUSASS}    { errorNum++; errorMsg("Illegal characters"); PrintErrorMsg("A", errormsg); }
{UNHANDLE}    { errorNum++; errorMsg("Mysterious character"); PrintErrorMsg("A", errormsg); }

%%
