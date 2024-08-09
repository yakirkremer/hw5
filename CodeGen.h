//
// Created by yakir on 8/6/2024.
//

#ifndef HW5_CODE_GEN_H
#define HW5_CODE_GEN_H

#include <string>
#include <vector>
#include <iostream>
#include "cg.hpp"
using namespace std;

class CodeGen {
    static CodeGen &instance ;
    CodeGen();
    int nextReg;
public:
    string freshVar();
    static CodeGen &getInstance();
    void genBranch(const string var,const string &label);
    void var_emit(const string &var);

};


#endif //HW5_CODE_GEN_H
