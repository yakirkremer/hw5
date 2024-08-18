//
// Created by yakir on 8/6/2024.
//
/*
#ifndef HW5_CODE_GEN_H
#define HW5_CODE_GEN_H

#include <string>
#include <vector>
#include <iostream>
#include "cg.hpp"
//#include "types.h"
#include "Exp.h"
class Exp;
class Terminal;
using namespace std;

class CodeGen {
    static CodeGen &instance ;
    CodeGen();
    int nextReg;
    string stack;
public:
    string freshVar();
    static CodeGen &getInstance();
    string stackInit();
    void genBranch(Exp * exp, bool genBool = false);
    void genExp(Exp *exp);
    //default expression bool = false, num = 0
    //returns containing register
    string genExp(string type);
    string genExp(Exp *exp1,Exp *exp2 ,string type, Terminal * op);
    string genNum(string num = "0", string type = "INT");
    string genStr(Exp *exp);
    string cast(string reg, string type, string cast_to);

    string genID(string reg, int offset);
    void assignID(string reg, string regID);
    string loadID(string pointer, string type);
    //string genAssign(string reg);
    void var_emit(const string &var);
    // exp1 <>==<=>= exp2
    string genBool(Exp *exp1, Exp *exp2, string op, Exp * exp);
    //True, False
    string genBool(Exp *exp);
    void genCall(string func, string pointer);
    string genReadi(string reg);
    string genZext(string reg, string type);
    string genTrunc(string reg, string type);

};


#endif //HW5_CODE_GEN_H
*/