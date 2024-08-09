//
// Created by yakir on 7/14/2024.
//

#ifndef HW3_TYPES_H
#define HW3_TYPES_H

#include <string>
#include <vector>
#include <iostream>
#include "hw3_output.hpp"
using namespace output;
using namespace std;
#include "tables.h"
#include "CodeGen.h"
#include "cg.hpp"
#include <list>

extern Scopes sym_table_scopes;
//extern CodeGen  &coder = CodeGen::getInstance();
//extern CodeBuffer &buffer = CodeBuffer::instance();

#define CHECK false

class Node;
#define YYSTYPE Node*
#define TYPE 0
#define DECL 1
#define EXP 2

#define ERROR_EXIT 0
#define DEFAULT 0


class Type;
class Exp;

class Node {
public:
    //vector<string> terminalVals;
    string terminalVal;

    Node(const string terminalVal = "");
    Node(const Node* terminal);

    virtual ~Node() = default;
};


class Program : public Node {
public:
    Program();
};

class Statements : public Node {


public:
    string statement_type;
    Statements(Node *statement);
    Statements(Node *statement, Node *statements, int yylineno);
};

class Statement : public Node {
    string inLabel;
    string outLabel;
    string code;
public:

    Statement(Node *type, Node *terminalVal, int yylineno);
    Statement( Node *terminalVal, Exp *exp, int yylineno, bool declare);
    Statement(Node *type, Node *terminalVal, Exp *exp, int yylineno, bool declare);
    Statement(Exp * exp, const string type, int yylineno);
    Statement(Node * terminalVal, int yylineno);
    string getInLabel();
    string getOutLabel();
};

class Call : public Node {

public:
    string type;
    Call(Node *terminal, Exp *exp, int yylineno);
};

class Type : public Node {

};

class Exp : public Node {

public:
    string code;
    string var;
    string type;
    string tmpLabel;
    bool is_var;
    int numVal =0;
    int size = 32;
    Exp(Exp* exp);
    //Num
    Exp(const string type, Node * terminal, int yylineno);
    //Casting
    Exp(Exp* exp, const string type, int yylineno);
    //Exp ADDITIVE Exp Exp MULTIPLICATIVE Exp
    Exp(const string op, Exp * exp1, Exp * exp2, int yylineno);
    //Exp ID
    Exp(const string terminalVal,Node * terminal ,bool is_var, int yylineno);

    void regInit(string op, string opr1, string opr2);


};

class ExpBool: public Exp {

public:
    ExpBool(Exp * exp,int yylineno);
    ExpBool(const string op, Exp * exp1, Exp * exp2, int yylineno);
    string trueLabel;
    string falseLabel;

};



Node* makeNode(int terminal_type,const string type, const string value);

#endif //HW3_TYPES_H
