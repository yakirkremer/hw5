//
// Created by yakir on 8/6/2024.
//

#include "CodeGen.h"
#include "tables.h"
#define MAX_STACK_SIZE 50

CodeGen::CodeGen() : nextReg(0) {}

CodeGen &CodeGen::getInstance() {
    static CodeGen instance;
    return instance;
}

string CodeGen::freshVar() {
    string reg =  "%t" + to_string(nextReg);
    nextReg++;
    return reg;
}

void CodeGen::genBranch(const std::string var, const std::string &label) {
    CodeBuffer::instance().emit("br i1 " + var + ", label %" + label + ", label %" + CodeBuffer::instance().freshLabel());
}

string CodeGen::genExp(Exp *exp) {
    string reg;
    if (exp->exp_type == "BOOL") {
        reg = genBool(exp);
    } else if (exp->exp_type == "BYTE" || exp->exp_type == "INT") {
        reg = genNum(exp->val);
    } else if (exp->exp_type == "STRING") {
        reg = genStr(exp);
    }
    return reg;

}

string CodeGen::genExp(Exp *exp1,Exp* exp2 , std::string op) {

    string var = freshVar();
    CodeBuffer::instance().emit(var + " = " + op + " i32 " + exp1->reg + ", " + exp2->reg);
    return var;
}

string CodeGen::genNum(string num) {
    string var = freshVar();
    CodeBuffer::instance().emit(var + " = add i32 0, " + num);
    return var;
}

string CodeGen::stackInit() {
    stack = freshVar();
    CodeBuffer::instance().emit(freshVar() + " = alloca i32, i32 " + to_string(MAX_STACK_SIZE));
    return stack;
}

string CodeGen::genID(string reg, int offset) {
    string pointer = freshVar();
    CodeBuffer::instance().emit(pointer + " = getelemtptr i32, i32* " + stack + ", i32 " + to_string(sym_table_scopes.scope->max_offset));
    CodeBuffer::instance().emit("store i32 " + reg + ", i32* " + pointer);
    return pointer;
}

string CodeGen::genStr(Exp *exp) {
    //CodeBuffer::instance().emit(freshVar() + " = getelementptr [" + to_string(exp->val.size() + 1) + " x i8], [" + to_string(exp->val.size() + 1) + " x i8]* @." + exp->val + ", i32 0, i32 0");
    return freshVar();
}

string CodeGen::genBool(Exp *exp) {
    int num = 0;
    if(exp->val == "TRUE") {
        return "1";
    }
    string var = freshVar();
    CodeBuffer::instance().emit(var + " = add i32 0, " + to_string(num));
    return var;
}

string CodeGen::genBool(Exp *exp1, Exp *exp2, std::string op) {
    string var = freshVar();
    CodeBuffer::instance().emit(var + " = icmp " + op + " i32 " + exp1->reg + ", " + exp2->reg);
    return var;
}



