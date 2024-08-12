//
// Created by yakir on 8/6/2024.
//

#ifndef HW5_CODE_GEN_H
#define HW5_CODE_GEN_H

#include <string>
#include <vector>
#include <iostream>
#include "cg.hpp"
#include "types.h"
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
    void genBranch(Exp * exp);
    void genExp(Exp *exp);
    string genExp(Exp *exp1,Exp *exp2 , Terminal * op);
    string genNum(string num = "0");
    string genStr(Exp *exp);
    string genBool(Exp *exp);
    string genID(string reg, int offset);
    string loadID(string pointer);
    //string genAssign(string reg);
    void var_emit(const string &var);
    string genBool(Exp *exp1, Exp *exp2, string op, Exp * exp);
    void genCall(string func, string pointer);

};


#endif //HW5_CODE_GEN_H
