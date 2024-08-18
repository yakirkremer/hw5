//
// Created by yakir on 8/6/2024.
//


/*
 * 11
 * 13
 * 15
 * 18
 * 19
 * 20-23
 * 24
 * 25
 */
/*
#include "CodeGen.h"
#include "tables.h"
#define MAX_STACK_SIZE 50

string type_parse(string type){
    if(type == "BOOL")
        return "i1";
    if(type == "BYTE")
        return "i8";

    return "i32";
}

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

void CodeGen::genBranch(Exp *exp, bool genBool) {

    if(exp->trueLabel == nullptr)
        dynamic_cast<Exp*>(exp)->trueLabel = new Label();
    if(exp->falseLabel == nullptr)
        dynamic_cast<Exp*>(exp)->falseLabel = new Label();
    if(exp->branch == true) {
        return;
    }
    string var = exp->reg;

    if (genBool == true){
        var = freshVar();
        exp->emit(var + " = icmp eq i32 "+ exp->reg + ", 1");
        string res = freshVar();
        exp->emitCode(res + " = zext i1 " + var + " to i32");
        var = res;
    }
    exp->emitBranch("br i1 " + var + ", label %" + exp->trueLabel->label + ", label %" + exp->falseLabel->label);
    exp->branch = true;



}

void CodeGen::genExp(Exp *exp) {


    if (exp->exp_type == "BOOL") {
        genBool(exp);
        //genBranch(exp,true);
    } else if (exp->exp_type == "BYTE" || exp->exp_type == "INT") {
        exp->reg = genNum(exp->val, exp->exp_type);
    } else if (exp->exp_type == "STRING") {
        exp->reg = genStr(exp);
    }
    return;

}

string CodeGen::genExp(std::string type) {
    string reg = freshVar();
    CodeBuffer::instance().emit(reg + " =  add  i32 0, 0");

    return reg;

}

string CodeGen::genExp(Exp *exp1,Exp* exp2 ,string type, Terminal * op) {
    string var = freshVar();
    if(numOperator(op->val) == "sdiv"){
        string divByZero = freshVar();
        CodeBuffer::instance().emit(divByZero + " = icmp eq i32 0 , " + exp2->reg);
        string cont = CodeBuffer::instance().freshLabel();
        CodeBuffer::instance().emit("br i1 " + divByZero + ", label %divByZero, label %" + cont);
        CodeBuffer::instance().emit(cont +":");
    }
    string size =" i32 ";// type_parse(type);
    CodeBuffer::instance().emit(var + " = " + numOperator(op->val) + size +  exp1->reg + ", " + exp2->reg);
    return var;

}

string CodeGen::genNum(string num, string type) {
    string var = freshVar();
    string size = "i32 ";

    if (type == "BYTE"  ) {
        size = "i32 ";
    }
    CodeBuffer::instance().emit(var + " = add " +  size + "0, " + num);

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

void CodeGen::assignID(std::string reg, string regID) {
    CodeBuffer::instance().emit("store i32 " + reg + ", i32* " + regID);
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




string CodeGen::cast(string reg, string type, string cast_to){
    return reg;
    string type1 = type_parse(type);
    string type2 = type_parse(cast_to);
    if(type1 == type2)
        return reg;

    string exp_reg = freshVar();
    CodeBuffer::instance().emit(exp_reg + " = zext " + type1 + reg + " to " + type2);
    return exp_reg;

}

void CodeGen::genCall(string func, string pointer) {
    string type ="";
    if(func == "print"){
        //pointer = freshVar();
        //CodeBuffer::instance().emit(freshVar() + " = zext i8* " + pointer + " to i32");
        type = " i8* ";
    } else {

        type = "i32 ";
    }
    CodeBuffer::instance().emit("call void @" + func + "(" + type + pointer + ")");
}

string CodeGen::genReadi(string param) {
    string reg = freshVar();
    CodeBuffer::instance().emit(reg + " = call i32 @readi (i32 " + param + ")");
    return reg;
}


string CodeGen::genBool(Exp *exp) {

    /*
    int num = 0;
    if(exp->val == "true")
        exp->val ="1";
    else if(exp->val == "false")
        exp->val ="0";
    else
        exp->val = exp->reg;
    *//*
    //exp->emit(exp->reg + " = add i32 0, " + exp->val);

    exp->reg = freshVar();

    exp->emit(exp->reg + " = add  i32 0, " + exp->val);
    //exp->trueLabel = new Label();
    //exp->falseLabel = new Label();
    //exp->curLabel = new Label("");

    return exp->reg;
}

string CodeGen::genBool(Exp *exp1, Exp *exp2, std::string op, Exp * exp) {
    exp->trueLabel = new Label();
    exp->falseLabel = new Label();
    //exp->curLabel = new Label("");
    string var = freshVar();
    string size = " i32 ";


    exp->emit(var + " = icmp " + relOperator(op) + size+ exp1->reg + ", " + exp2->reg);
    string res = freshVar();
    exp->emitCode(res + " = zext i1 " + var + " to i32");


    return res;
}

string CodeGen::loadID(std::string pointer, string type) {
    string var = freshVar();
    CodeBuffer::instance().emit(var + " = load i32, i32* " + pointer);
    //var = genTrunc(var, type);
    //var = cast(var, type, "INT");
    return var;
}

string CodeGen::genZext(string reg, string type) {
    return reg;
    string exp_reg = freshVar();
    if(type == "BOOL"){
        CodeBuffer::instance().emit(exp_reg + " = zext i1 " + reg + " to i32");
    }
    if(type == "BYTE"){
        CodeBuffer::instance().emit(exp_reg + " = zext i8 " + reg + " to i32");
    }
    return exp_reg;
}

string CodeGen::genTrunc(string reg, string type) {
    string exp_reg = reg + "b";
    if(type == "BOOL"){
        CodeBuffer::instance().emit(exp_reg + " = trunc i32 " + reg + " to i1");
        return exp_reg;
    }
    if(type == "BYTE"){
        CodeBuffer::instance().emit(exp_reg + " = trunc i32 " + reg + " to i8");
        return exp_reg;
    }
    return reg;
}


*/
