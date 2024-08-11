%{

/* Declarations section */
#include "types.h"
#include "hw3_output.hpp"
using namespace output;
#include "parser.tab.hpp"

%}

%option noyywrap
%option yylineno

whitespace [ \t\n\r]
printable [ -~]
escape [\n]

%%
int                     yylval=new Terminal("INT");    return INT;
byte                    yylval=new Terminal("BYTE");    return BYTE;
b                       yylval=new Terminal(yytext);    return B;
bool                    yylval=new Terminal("BOOL");    return BOOL;
and                     yylval=new Terminal(yytext);  return AND;
or                      yylval=new Terminal(yytext);  return OR;
not                     yylval=new Terminal(yytext);  return NOT;
true                    yylval=new Terminal(yytext);  return TRUE;
false                  yylval=new Terminal(yytext);   return FALSE;
return                  yylval=new Terminal(yytext);  return RETURN;
if                      yylval=new Terminal(yytext);  return IF;
else                    yylval=new Terminal(yytext);  return ELSE;
while                  yylval=new Terminal(yytext);   return WHILE;
break                  yylval=new Terminal(yytext);   return BREAK;
continue               yylval=new Terminal(yytext);   return CONTINUE;
;                       yylval=new Terminal(yytext);  return SC;
\(                      yylval=new Terminal(yytext);  return LPAREN;
\)                     yylval=new Terminal(yytext);   return RPAREN;
\{                      yylval=new Terminal(yytext);  return LBRACE;
\}                     yylval=new Terminal(yytext);   return RBRACE;
=                      yylval=new Terminal(yytext);   return ASSIGN;
"<="|">="|<|>          yylval=new Terminal(yytext);  return RELOP;
==|!=                  yylval=new Terminal(yytext);   return EQUALITY;
[-+]                   yylval=new Terminal(yytext);   return ADDITIVE;
[*/]                   yylval=new Terminal(yytext);   return MULTIPLICATIVE;
[a-zA-Z][a-zA-Z0-9]*     yylval=new Terminal(yytext); return ID;
[1-9][0-9]*|0          yylval=new Terminal(yytext);   return NUM;
\/\/[^\r\n]*            ;
\"([ !#-\[\]-~]|\\[\\\"nrt0]|\\x[0-7][0-9A-Fa-f])+\"  yylval=new Terminal(yytext);    return STRING;
{whitespace}            ;
.                       {errorLex(yylineno); exit(1);}
%%