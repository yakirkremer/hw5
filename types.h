//
// Created by yakir on 8/9/2024.
//
/*
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

#define CHECK false
#define YYSTYPE Terminal*
#define ASSIGN_ONLY 0
#define DEFINE_ONLY 1
#define DEFINE_ASSIGN 2
#define CALL 3

#define ERROR_EXIT -1

class Branch;
string freshLabel();
class Label{
public:
    string label;
    string code;
    Label *trueLabel;
    Label *falseLabel;
    Label();
    Label(string label);
    virtual ~Label() = default;


};

class Branch{
public:
    string code;
    Label *trueLabel;
    Label *falseLabel;
};

class Exp;
class Terminal {
public:
    string val;
    Terminal(const string terminalVal);
    Terminal();

    virtual ~Terminal() = default;
};


/*class nextLabel : public Terminal{
public:
    string label;
    nextLabel(){label = freshLabel();}
    nextLabel(string label):label(label){}
};*/
/*
class Exp :public Terminal{

public:
    Label *trueLabel = nullptr;
    Label *falseLabel = nullptr;
    Label * nextLabel;
    string code;
    string branchCode;
    string reg;
    string exp_type;
    bool branch = false;
    string& gen(string code1);
    string& gen(string code1, string code2);
    Exp(Terminal * exp1,Terminal * exp2,string type, string op,Terminal * opTerminal ,int line);
    Exp(Terminal* exp,string new_type ,int line);
    Exp(Exp*);
     void emit(string code);
        void emitBranch(string code1);
        void emitCode(string code1);
     //string getCode(){return curLabel->code;}
     string getCode(){return code;}
     string getBranchCode(){return branchCode;}

};

class ExpBool :public Exp{
public:
    ExpBool(Terminal * exp, int yylineno);
    void buildCode();
};

class Call : public Terminal {

public:
    string reg;
    string func_type;
    Call(Terminal *terminal, Terminal *exp, int yylineno);
};

class Statement:public Terminal{

public:
    Label *trueLabel;
    Label *falseLabel;
    string nextLabel;
    Exp *exp;
    Statement(int mode, Terminal * id, Terminal  * type, Terminal * exp, int line);
    Statement(Terminal * expBool);
    Statement(string nextLabel):nextLabel(nextLabel){}
};

string num_type(const string type);

*/
#endif //HW3_NEW_TYPES_H
