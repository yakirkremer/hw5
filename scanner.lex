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
int                     yylval=new Node("INT");    return INT;
byte                    yylval=new Node("BYTE");    return BYTE;
b                       yylval=new Node(yytext);    return B;
bool                    yylval=new Node("BOOL");    return BOOL;
and                     yylval=new Node(yytext);  return AND;
or                      yylval=new Node(yytext);  return OR;
not                     yylval=new Node(yytext);  return NOT;
true                    yylval=new Node(yytext);  return TRUE;
false                  yylval=new Node(yytext);   return FALSE;
return                  yylval=new Node(yytext);  return RETURN;
if                      yylval=new Node(yytext);  return IF;
else                    yylval=new Node(yytext);  return ELSE;
while                  yylval=new Node(yytext);   return WHILE;
break                  yylval=new Node(yytext);   return BREAK;
continue               yylval=new Node(yytext);   return CONTINUE;
;                       yylval=new Node(yytext);  return SC;
\(                      yylval=new Node(yytext);  return LPAREN;
\)                     yylval=new Node(yytext);   return RPAREN;
\{                      yylval=new Node(yytext);  return LBRACE;
\}                     yylval=new Node(yytext);   return RBRACE;
=                      yylval=new Node(yytext);   return ASSIGN;
"<="|">="|<|>          yylval=new Node(yytext);  return RELOP;
==|!=                  yylval=new Node(yytext);   return EQUALITY;
[-+]                   yylval=new Node(yytext);   return ADDITIVE;
[*/]                   yylval=new Node(yytext);   return MULTIPLICATIVE;
[a-zA-Z][a-zA-Z0-9]*     yylval=new Node(yytext); return ID;
[1-9][0-9]*|0          yylval=new Node(yytext);   return NUM;
\/\/[^\r\n]*            ;
\"([ !#-\[\]-~]|\\[\\\"nrt0]|\\x[0-7][0-9A-Fa-f])+\"  yylval=new Node(yytext);    return STRING;
{whitespace}            ;
.                       {errorLex(yylineno); exit(1);}
%%