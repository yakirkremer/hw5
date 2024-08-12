//
// Created by yakir on 8/6/2024.
//

#include "CodeGen.h"
#include "tables.h"
#define MAX_STACK_SIZE 50

string relOperator(string op) {
    if (op == "==") {
        return "eq";
    } else if (op == "!=") {
        return "ne";
    } else if (op == "<") {
        return "slt";
    } else if (op == ">") {
        return "sgt";
    } else if (op == "<=") {
        return "sle";
    } else if (op == ">=") {
        return "sge";
    }
    return "";
}

string numOperator(string op) {
    if (op == "+") {
        return "add";
    } else if (op == "-") {
        return "sub";
    } else if (op == "*") {
        return "mul";
    } else if (op == "/") {
        return "sdiv";
    }
    return "";
}

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

void CodeGen::genBranch(Exp *exp) {
    exp->emit("br i1 " + exp->reg + ", label %" + exp->trueLabel->label + ", label %" + exp->falseLabel->label);
}

void CodeGen::genExp(Exp *exp) {
    string reg;
    if (exp->exp_type == "BOOL") {
        exp->reg = genBool(exp);
        CodeGen::getInstance().genBranch(exp);
    } else if (exp->exp_type == "BYTE" || exp->exp_type == "INT") {
        exp->reg = genNum(exp->val);
    } else if (exp->exp_type == "STRING") {
        exp->reg = genStr(exp);
    }
    return;

}

string CodeGen::genExp(Exp *exp1,Exp* exp2 , Terminal * op) {
    string var = freshVar();
    CodeBuffer::instance().emit(var + " = " + numOperator(op->val) + " i32 " + exp1->reg + ", " + exp2->reg);
    return var;
}

string CodeGen::genNum(string num) {
    string var = freshVar();
    CodeBuffer::instance().emit(var + " = add i32 0, " + num);
    return var;
}

string CodeGen::stackInit() {
    stack = freshVar();
    CodeBuffer::instance().emit(stack + " = alloca i32, i32 " + to_string(MAX_STACK_SIZE));
    return stack;
}

string CodeGen::genID(string reg, int offset) {
    string pointer = freshVar();
    CodeBuffer::instance().emit(pointer + " = getelementptr i32, i32* " + stack + ", i32 " + to_string(sym_table_scopes.scope->max_offset));
    CodeBuffer::instance().emit("store i32 " + reg + ", i32* " + pointer);
    return pointer;
}


void removeQuotes(string &str) {
    str = str.substr(1, str.size() - 2); // remove quotes ["str"

}
void addQuotes(string &str) {
    str = "\"" + str + "\"";
}

string CodeGen::genStr(Exp *exp) {
    //exp->val += "\\00";
    string var = freshVar();
    string globalVar = var.substr(1);
    removeQuotes(exp->val);
    exp->val+= "\\00";
    addQuotes(exp->val);
    CodeBuffer::instance().emit(var + ".ptr = getelementptr [" + to_string(exp->val.size()-4) + " x i8], [" + to_string(exp->val.size()-4) + " x i8]* @" + globalVar + ", i32 0, i32 0");
    CodeBuffer::instance().emitGlobal("@" + globalVar + " = constant [" +to_string(exp->val.size()-4) +  " x i8] c" +exp->val );

    return  var + ".ptr";
}

void CodeGen::genCall(string func, string pointer) {
    CodeBuffer::instance().emit("call void @" + func + "(i8* " + pointer + ")");
}

string CodeGen::genBool(Exp *exp) {
    int num = 0;
    if(exp->val == "true")
        num =1;
    exp->trueLabel = new Label();
    exp->falseLabel = new Label();
    exp->curLabel = new Label("");
    string var = freshVar();
    exp->emit(var + " = icmp eq i32 1, " + to_string(num));
    return var;
}

string CodeGen::genBool(Exp *exp1, Exp *exp2, std::string op, Exp * exp) {
    exp->trueLabel = new Label();
    exp->falseLabel = new Label();
    exp->curLabel = new Label("");
    string var = freshVar();
    exp->emit(var + " = icmp " + relOperator(op) + " i32 "+ exp1->reg + ", " + exp2->reg);
    return var;
}

string CodeGen::loadID(std::string pointer) {
    string var = freshVar();
    CodeBuffer::instance().emit(var + " = load i32, i32* " + pointer);
    return var;
}



