%{

/* Declarations section */
//#include "types.h"
#include "Exp.h"
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
int                     yylval=new Exp("INT");    return INT;
byte                    yylval=new Exp("BYTE");    return BYTE;
b                       yylval=new Exp(yytext);    return B;
bool                    yylval=new Exp("BOOL");    return BOOL;
and                     yylval=new Exp(yytext);  return AND;
or                      yylval=new Exp(yytext);  return OR;
not                     yylval=new Exp(yytext);  return NOT;
true                    yylval=new Exp("1");  return TRUE;
false                  yylval=new Exp("0");   return FALSE;
return                  yylval=new Exp(yytext);  return RETURN;
if                      yylval=new Exp(yytext);  return IF;
else                    yylval=new Exp(yytext);  return ELSE;
while                  yylval=new Exp(yytext);   return WHILE;
break                  yylval=new Exp(yytext);   return BREAK;
continue               yylval=new Exp(yytext);   return CONTINUE;
;                       yylval=new Exp(yytext);  return SC;
\(                      yylval=new Exp(yytext);  return LPAREN;
\)                     yylval=new Exp(yytext);   return RPAREN;
\{                      yylval=new Exp(yytext);  return LBRACE;
\}                     yylval=new Exp(yytext);   return RBRACE;
=                      yylval=new Exp(yytext);   return ASSIGN;
"<="|">="|<|>          yylval=new Exp(yytext);  return RELOP;
==|!=                  yylval=new Exp(yytext);   return EQUALITY;
[-+]                   yylval=new Exp(yytext);   return ADDITIVE;
[*/]                   yylval=new Exp(yytext);   return MULTIPLICATIVE;
[a-zA-Z][a-zA-Z0-9]*     yylval=new Exp(yytext); return ID;
[1-9][0-9]*|0          yylval=new Exp(yytext);   return NUM;
\/\/[^\r\n]*            ;
\"([ !#-\[\]-~]|\\[\\\"nrt0]|\\x[0-7][0-9A-Fa-f])+\"  yylval=new Exp(yytext);    return STRING;
{whitespace}            ;
.                       {errorLex(yylineno); exit(1);}
%%