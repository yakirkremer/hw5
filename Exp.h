//
// Created by yakir on 8/15/2024.
//

#ifndef HW5_EXP_H
#define HW5_EXP_H

#include "tables.h"
#include "cg.hpp"
#include <string>
#include "hw3_output.hpp"



using namespace output;
/*
class Terminal{
    string val;
    Terminal(string val){
        this->val = val;
    }

};*/

#define YYSTYPE Exp*
#define ERROR_EXIT -1
#define MAX_STACK_SIZE 50
#define ASSIGN_ONLY 1
#define DEFINE_ASSIGN 2
extern Scopes sym_table_scopes;



class Exp{
public:
    string val;
string type;
string code;
string reg;
Exp(string val){
    this->val = val;
}
Exp(){
    this->val = "";
}
    virtual ~Exp() = default;
    virtual string getReg(){return reg;}
    virtual string getType(){return type;
}
    virtual string getCode(){return code;}
    virtual void emit(string code1){code = code + "\n" + code1;}
    virtual bool check_exp(){return true;};
};

string genBool(Exp * exp, int line);
string genBoolAssign(Exp * exp, int line);
class ExpID:public Exp{
public:
ExpID(string id,int line);
};

class ExpNum:public Exp{
public:
    ExpNum(Exp* num, string type, int line);
    ExpNum(Exp * exp1,Exp * exp2,Exp * op, int line);
    bool check_exp(string op, Exp * exp1, Exp * exp2, int line);

};

class ExpStr:public Exp{
public:
    ExpStr(Exp * str, int line);
};

class ExpBool:public Exp{
public:
    bool branched = false;
    bool icmped = false;
    bool notted = false;
    string valCode;
    string bool_var;
    string icmp;
    string branch;
    string trueLabel = "";
    string falseLabel = "";
    string nextLabel = "";
    string phi;
    ExpBool(string exp_reg,string code1, int line);
    //true false
    ExpBool(Exp * boolean, int line);
    //and or
    ExpBool(Exp * exp1,Exp * exp2,string op, int line);
    //not
    ExpBool(Exp * exp1,string op, int line);
    //relop
    ExpBool(Exp * exp1,Exp * exp2,Exp * op, int line);
    void genBranch();
    void genIcmp();
    void emitAssign(string code1);
    bool check_exp(string op, Exp * exp1, Exp * exp2, int line);
};

class AssignStatement : public Exp{
public:
    AssignStatement(string type, string id,Exp * exp, int line);
    AssignStatement(string id, Exp * exp, int line);
    bool check_exp(string id, string exp_type, int line, int mode);
};

class DefineStatement : public Exp{
public:
    DefineStatement(string type ,string id, int line);
    bool check_exp(string id, int line);
};

class ExpCast:public Exp{
public:
    ExpCast(string type,Exp * exp, int line);
    bool check_exp(string type_s, Exp * exp, int line);
};

class ExpCall:public Exp{
    ExpCall(string id, int line);
};


class IfStatement:public Exp{
public:
    IfStatement(string label, Exp * exp,int line);
};

class IfElseStatement:public Exp{
public:
    IfElseStatement(string label, Exp * exp,int line);
    string nextLabel;

};

class WhileStatement:public Exp{
public:
    WhileStatement(string label, Exp * exp,int line);
};

class Call : public Exp {

public:
    string func_type;
    Call(Exp * exp1 , Exp *exp2, int yylineno);
};

class Vars{
    static Vars &instance ;
    int nextReg;
    string stack;
    Vars(){
        nextReg = 0;
    }
public:
    string stackInit();
    static Vars &getInstance();
    string freshvar();
    string getStack(){return stack;}
};

#endif //HW5_EXP_H
