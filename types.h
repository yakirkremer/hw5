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

extern Scopes sym_table_scopes;

#define CHECK false

class Node;
#define YYSTYPE Node*
#define TYPE 0
#define DECL 1
#define EXP 2

#define ERROR_EXIT 0

class Type;
class Exp;

class Node {
public:
    //vector<string> names;
    string name;

    Node(const string name = "");
    Node(const Node* node);

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
public:
    Statement(Node *type, Node *name, int yylineno);
    Statement( Node *name, Exp *exp, int yylineno, bool declare);
    Statement(Node *type, Node *name, Exp *exp, int yylineno, bool declare);
    Statement(Exp * exp, const string type, int yylineno);
};

class Call : public Node {

public:
    string type;
    Call(Node *node, Exp *exp, int yylineno);
};

class Type : public Node {

};

class Exp : public Node {

public:
    string type;
    bool is_var;
    int val =0;
    Exp(Exp * exp);
    //Casting
    Exp(Exp* exp, const string type, int yylineno);
    //NUM NUM B STRING TRUE FALSE
    Exp(const string type, Node * node,int yylineno);
    // Not EXP ,AND,OR,RELOP, EQUALITY
    Exp(const string type,const string op, Exp * exp1, Exp * exp2, int yylineno);
    //Exp ADDITIVE Exp Exp MULTIPLICATIVE Exp
    Exp(const string op, Exp * exp1, Exp * exp2, int yylineno);
    //Exp ID
    Exp(const string name,Node * node ,bool is_var, int yylineno);

};

class ExpBool: public Exp {
public:
    ExpBool(Exp * exp, int yylineno);

};



Node* makeNode(int node_type,const string type, const string value);

#endif //HW3_TYPES_H
