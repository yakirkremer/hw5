//
// Created by yakir on 8/15/2024.
//

#include "Exp.h"





void removeQuotes(string &str) {
    str = str.substr(1, str.size() - 2); // remove quotes ["str"

}
void addQuotes(string &str) {
    str = "\"" + str + "\"";
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

string loadID(string pointer, string type,Exp) {
    string var = Vars::getInstance().freshvar();
    CodeBuffer::instance().emit(var + " = load i32, i32* " + pointer);
    //var = genTrunc(var, type);
    //var = cast(var, type, "INT");
    return var;
}

bool not_num(const string type) {
    if(type != "INT"  && type!= "BYTE" && type != "num"&& type!= "byte"&& type!= "int")
        return true;
    return false;
}
string num_type(const string type){
    if(type == "INT" || type == "BYTE")
        return "num";
    return type;
}



ExpID::ExpID(string id, int line) {
    if(sym_table_scopes.variable_exists(id) == false){
        errorUndef(line,id);
        exit(ERROR_EXIT);
    }
    type = sym_table_scopes.get_symbol(id)->get_type();
    string pointer = sym_table_scopes.get_symbol(id)->pointer;
    reg = Vars::getInstance().freshvar();

    emit(reg + " = load i32, i32* " + pointer);
}

ExpNum::ExpNum(Exp *num, std::string type, int line):Exp(num->val) {
    this->type = type;
    check_exp("",num,num,line);
    reg = Vars::getInstance().freshvar();
    emit(reg + " = add i32 0, " + num->val);
}

ExpNum::ExpNum(Exp *exp1, Exp *exp2,Exp * op , int line):Exp("0") {
    emit(exp1->getCode());
    emit(exp2->getCode());
    this->type = "INT";
    check_exp(numOperator(op->val),exp1,exp2,line);
    if (exp1->type != "INT" && exp2->type != "INT") {
        this->type = "BYTE";
    }



    reg = Vars::getInstance().freshvar();
    if(numOperator(op->val) == "sdiv"){
        string divByZero = Vars::getInstance().freshvar();
        emit(divByZero + " = icmp eq i32 0 , " + exp2->reg);
        string cont = CodeBuffer::instance().freshLabel();
        emit("br i1 " + divByZero + ", label %divByZero, label %" + cont);
        emit(cont +":");
    }
    emit(reg + " = " + numOperator(op->val) + " i32 " + exp1->reg + ", " + exp2->reg);
    if(this->type == "BYTE"){
        string temp = Vars::getInstance().freshvar();
        emit(temp + " = trunc i32 " + reg + " to  i8");
        reg = Vars::getInstance().freshvar();
        emit(reg + " = zext i8 " + temp + " to i32");
    }
}

bool ExpNum::check_exp(string op, Exp *exp1, Exp *exp2, int line){
    if(op == "add" || op == "mul"){
        if(not_num(exp1->type) || not_num(exp2->type)){
            errorMismatch(line);
            exit(ERROR_EXIT);
        }
        return true;
    }

    if(type == "BYTE"){
        int numVal;
        try {
            numVal = stoi(exp1->val);
        }
        catch (exception &e){
            errorByteTooLarge(line,exp1->val);
            exit(ERROR_EXIT);
        }
        if(numVal > 255) {
            errorByteTooLarge(line, exp1->val);
            exit(ERROR_EXIT);
        }

    }
    return true;
}

ExpStr::ExpStr(Exp *str, int line) {
    type = "STRING";
    string var = Vars::getInstance().freshvar();
    string globalVar = var.substr(1);
    removeQuotes(str->val);
    str->val+= "\\00";
    addQuotes(str->val);
    emit(var + ".ptr = getelementptr [" + to_string(str->val.size()-4) + " x i8], [" + to_string(str->val.size()-4) + " x i8]* @" + globalVar + ", i32 0, i32 0");
    CodeBuffer::instance().emitGlobal("@" + globalVar + " = constant [" +to_string(str->val.size()-4) +  " x i8] c" +str->val );
    reg =  var + ".ptr";
}

void ExpBool::emitAssign(string code1) {
    //code += code1 + "\n";
    //valCode += code1 + "\n";
}

bool ExpBool::check_exp(string op, Exp * exp1, Exp * exp2, int line) {
    if(op == "and" || op == "or" || op == "not"){
        if(exp1->type != "BOOL" || exp2->type != "BOOL"){
            errorMismatch(line);
            exit(ERROR_EXIT);
        }
    }
    if((op == "relop") ) {
        if (not_num(exp1->type) || not_num(exp2->type)) {
            errorMismatch(line);
            exit(ERROR_EXIT);
        }

    }
    return true;
}

ExpBool::ExpBool(string  exp_reg,string code1, int line){
    type = "BOOL";
    emitAssign(code1);
    emit(code1);
    reg = Vars::getInstance().freshvar();
    bool_var = Vars::getInstance().freshvar();
    emit(reg + " = add i32 0, " + exp_reg);
    icmp = (" = icmp eq i32 " + reg + ", 1");
   // emitAssign(bool_var + icmp);
}


//True False
ExpBool::ExpBool(Exp * boolean, int line) {
    val = boolean->val;
    type = "BOOL";
    reg = Vars::getInstance().freshvar();
    bool_var = Vars::getInstance().freshvar();
    icmped = true;
    //emitAssign(reg + " = add i32 0, " + boolean->val);
    //emit(reg + " = add i32 0, " + boolean->val);
    //icmp = (" = icmp eq i32 " + reg + ", 1");
    //emitAssign(bool_var + icmp);

}

/*
ExpBool::ExpBool(Exp * boolean, int line) {
    type = "BOOL";
    reg = Vars::getInstance().freshvar();
    bool_var = Vars::getInstance().freshvar();
    emitAssign(reg + " = add i32 0, " + boolean->val);
    emit(reg + " = add i32 0, " + boolean->val);
    emit(bool_var + " = icmp eq i32 " + reg + ", 1");
    trueLabel = CodeBuffer::instance().freshLabel();
    falseLabel = CodeBuffer::instance().freshLabel();
    nextLabel = CodeBuffer::instance().freshLabel();
    emit("br i1 " + bool_var + ", label %" + trueLabel + ", label %" + falseLabel);
    emit(trueLabel + ":");
    string true_var = Vars::getInstance().freshvar();
    emit(true_var + " = add i32 0, 1");
    emit("br label %" + nextLabel);
    emit(falseLabel + ":");
    string false_var = Vars::getInstance().freshvar();
    emit(false_var + " = add i32 0, 0");
    emit("br label %" + nextLabel);
    emit(nextLabel + ":");
    reg = Vars::getInstance().freshvar();
    emit(reg + " = phi i32 [" + true_var + ", %" + trueLabel + "], [" + false_var + ", %" + falseLabel + "]");

    //emitAssign(bool_var + icmp);

}*/

ExpBool::ExpBool(Exp *exp1, std::string op, int line) {

    type = "BOOL";
    check_exp("not",exp1,exp1,line);
    ExpBool *bool1 = dynamic_cast<ExpBool *>(exp1);
    icmped = bool1->icmped;
    val = bool1->val;
    notted = not(bool1->notted);
    //bool1->genBranch();
    code = bool1->getCode();
    valCode = bool1->valCode;
    bool_var = bool1->bool_var;
    icmp = bool1->icmp;
    branch = bool1->branch;
    branched = bool1->branched;
    trueLabel = bool1->falseLabel;
    falseLabel = bool1->trueLabel;
    reg = bool1->reg;
    //emit(exp1->getCode());
}
/*
ExpBool::ExpBool(Exp *exp1, Exp *exp2, string op, int line) {
    type = "BOOL";
    ExpBool *bool1 = dynamic_cast<ExpBool *>(exp1);
    ExpBool *bool2 = dynamic_cast<ExpBool *>(exp2);
    trueLabel = CodeBuffer::instance().freshLabel();
    falseLabel = CodeBuffer::instance().freshLabel();
    nextLabel = CodeBuffer::instance().freshLabel();
    if (op == "and") {
        emit(bool1->getCode());

        emit(bool2->getCode());


    }
}*/
//BOOL And Or Not

ExpBool::ExpBool(Exp *exp1, Exp *exp2, string op, int line) {
    type = "BOOL";
    check_exp(op,exp1,exp2,line);
    ExpBool *bool1 = dynamic_cast<ExpBool *>(exp1);
    ExpBool *bool2 = dynamic_cast<ExpBool *>(exp2);

    branched = true;

    bool1->genBranch();
    bool2->genBranch();

//AND

    emitAssign(bool2->valCode);
    emitAssign(bool1->valCode);
    if (op == "and") {
        trueLabel = bool2->trueLabel;
        falseLabel = bool1->falseLabel;

        emit(bool1->getCode());


        emit(bool1->trueLabel + ":");

        emit(bool2->getCode());

        emit(bool2->falseLabel + ":");

        emit("br label %" + falseLabel);

        //emit(trueLabel + ":");
        //emit("br label %" + bool1->falseLabel);
        // cout << code << endl;
    }
//OR
    else if (op == "or") {
        trueLabel = bool1->trueLabel;
        falseLabel = bool2->falseLabel;


        emit(bool1->getCode());

        emit(bool1->falseLabel + ":");

        emit(bool2->getCode());

        emit(bool2->trueLabel + ":");

        emit("br label %" + trueLabel);

    }


}

//Relop
ExpBool::ExpBool(Exp *exp1, Exp *exp2, Exp *op, int line) {
    type = "BOOL";
    check_exp("relop",exp1,exp2,line);
    Exp * bool1 = dynamic_cast<Exp*>(exp1);
    Exp * bool2 = dynamic_cast<Exp*>(exp2);
    emit(bool1->getCode());
    emit(bool2->getCode());
    emitAssign(bool1->getCode());
    emitAssign(bool2->getCode());
    string rel = relOperator(op->val);
    bool_var = Vars::getInstance().freshvar();
    icmp = (" = icmp " + rel + " i32 " + bool1->reg + ", " + bool2->reg);
    //emitAssign(bool_var + icmp);



}

void ExpBool::genIcmp() {
    if(icmped == true)
        return;
    icmped = true;
    emit(bool_var + icmp);
    emitAssign(bool_var + icmp);
    reg = bool_var;
}
void ExpBool::genBranch() {
    if(branched == true)
        return;
    branched = true;
    //bool_var = Vars::getInstance().freshvar();
    if(trueLabel == "")
        trueLabel = CodeBuffer::instance().freshLabel();
    if(falseLabel == "")
        falseLabel = CodeBuffer::instance().freshLabel();
    if(val == "1")
        branch = ("br label %" + trueLabel );
    else if(val == "0")
        branch = ("br label %" + falseLabel );
    else
        branch = ("br i1 " + bool_var + ", label %" + trueLabel + ", label %" + falseLabel);

    if(notted == true){
        string temp = trueLabel;
        trueLabel = falseLabel;
        falseLabel = temp;
    }
    genIcmp();
    emit(branch);
    //emit(trueLabel + ":");
    //cout<<code<<endl;
}

bool AssignStatement::check_exp(std::string id, string exp_type, int line, int mode) {
    if(mode == ASSIGN_ONLY) {
        if (sym_table_scopes.variable_exists(id) == false) {
            errorUndef(line, id);
            exit(ERROR_EXIT);
        }
        string type_s = sym_table_scopes.get_symbol(id)->get_type();
        if (exp_type != type_s) {
            if (!not_num(exp_type) && !not_num(type_s) && type_s != "BYTE") {
                return true;
            }
            errorMismatch(line);
            exit(ERROR_EXIT);
        }
    }
    else if(mode == DEFINE_ASSIGN) {
            if (sym_table_scopes.symbol_exists(id) == true) {
                errorDef(line, id);
                exit(ERROR_EXIT);
            }
        if (exp_type != type) {

            if (((not_num(exp_type) && not_num(type)) || type == "BYTE")) {
                errorMismatch(line);
                exit(ERROR_EXIT);

            }
        }
    }
    return true;
}

//Statement: Type ID ASSIGN Exp SC
AssignStatement::AssignStatement( std::string type,std::string id, Exp *exp, int line) {
    this->type = type;

    check_exp(id,exp->type,line,DEFINE_ASSIGN);
    Exp * assign_exp = dynamic_cast<Exp*>(exp);
    if(exp->type == "BOOL"){
        /*
        //dynamic_cast<ExpBool*>(exp)->emitAssign(dynamic_cast<ExpBool*>(exp)->valCode);
        dynamic_cast<ExpBool*>(exp)->genIcmp();

        string oldReg = exp->reg;
        exp->reg = Vars::getInstance().freshvar();
        dynamic_cast<ExpBool*>(exp)->emitAssign(exp->reg + " = zext i1 " + oldReg + " to i32");
        emit(dynamic_cast<ExpBool*>(exp)->valCode);*/
        dynamic_cast<ExpBool*>(exp)->genBranch();
        CodeBuffer::instance().emit(exp->getCode());
        CodeBuffer::instance().emit(dynamic_cast<ExpBool*>(exp)->trueLabel + ":");
        exp-> reg = genBoolAssign(exp,line);
    }
    else
        emit(assign_exp->getCode());
    if(sym_table_scopes.variable_exists(id) == true){
        output::errorDef(line,id);
        exit(ERROR_EXIT);
    }
    string pointer = Vars::getInstance().freshvar();
    sym_table_scopes.add_symbol(id, type,1 ,pointer, false,"");
    emit(pointer + " = getelementptr i32, i32* " + Vars::getInstance().getStack() + ", i32 " + to_string(sym_table_scopes.get_symbol(id)->offset));
    emit("store i32 " + exp->reg + ", i32* " + pointer);
    CodeBuffer::instance().emit(getCode());
}



//Statement: ID ASSIGN Exp SC
AssignStatement::AssignStatement( std::string id, Exp *exp, int line) {
    check_exp(id,exp->type,line ,ASSIGN_ONLY);
    Exp * assign_exp = dynamic_cast<Exp*>(exp);
    if(exp->type == "BOOL"){
        /*
        //dynamic_cast<ExpBool*>(exp)->emitAssign(dynamic_cast<ExpBool*>(exp)->valCode);
        dynamic_cast<ExpBool*>(exp)->genIcmp();

        string oldReg = exp->reg;
        exp->reg = Vars::getInstance().freshvar();
        dynamic_cast<ExpBool*>(exp)->emitAssign(exp->reg + " = zext i1 " + oldReg + " to i32");
        emit(dynamic_cast<ExpBool*>(exp)->valCode);*/
        dynamic_cast<ExpBool*>(exp)->genBranch();
        CodeBuffer::instance().emit(exp->getCode());
        CodeBuffer::instance().emit(dynamic_cast<ExpBool*>(exp)->trueLabel + ":");
        exp-> reg = genBoolAssign(exp,line);
    }
    else
        emit(assign_exp->getCode());

    string pointer = sym_table_scopes.get_symbol(id)->pointer;
    emit("store i32 " + exp->reg + ", i32* " + pointer);
    CodeBuffer::instance().emit(getCode());

}


bool DefineStatement::check_exp(std::string id, int line) {
    if (sym_table_scopes.symbol_exists(id) == true) {
        errorDef(line, id);
        exit(ERROR_EXIT);
    }
    return true;
}

//Statement: Type ID SC
DefineStatement::DefineStatement( std::string type,std::string id, int line) {
    check_exp(id, line);
    string pointer = Vars::getInstance().freshvar();
    sym_table_scopes.add_symbol(id, type,1 ,pointer, false,"");
    emit(pointer + " = getelementptr i32, i32* " + Vars::getInstance().getStack() + ", i32 " + to_string(sym_table_scopes.get_symbol(id)->offset));
    emit("store i32 0, i32* " + pointer);
    CodeBuffer::instance().emit(getCode());

}




Call::Call(Exp * terminal, Exp *exp, int line) {
    if(sym_table_scopes.function_exists(terminal->val) == false){
        errorUndefFunc(line,terminal->val);
        exit(ERROR_EXIT);
    }
    this->func_type = sym_table_scopes.get_symbol(terminal->val)->get_type();
    type = func_type;
    if(num_type(dynamic_cast<Exp*>(exp)->type) != sym_table_scopes.get_symbol(terminal->val)->get_arg_type()){
        errorPrototypeMismatch(line,terminal->val,sym_table_scopes.get_symbol(terminal->val)->get_arg_type_for_print());
        exit(ERROR_EXIT);
    }
    if(terminal->val != "readi") {
        string arg_type = " i32 ";
        if(terminal-> val == "print")
            arg_type = " i8* ";
        emit(dynamic_cast<Exp*>(exp)->getCode());
        emit("call void @" + terminal->val + "(" + arg_type + dynamic_cast<Exp*>(exp)->reg + ")");
    }
    else {
        emit(dynamic_cast<Exp*>(exp)->getCode());
        this->reg = Vars::getInstance().freshvar();
        emit(reg + " = call i32 @readi (i32 " + dynamic_cast<Exp*>(exp)->reg + ")");
    }
}


bool ExpCast::check_exp(std::string type_s, Exp *exp, int line) {
    if(not_num(exp->type) || not_num(type_s)) {
        errorMismatch(line);
        exit(ERROR_EXIT);
    }
    return true;
}

ExpCast::ExpCast(std::string type, Exp *exp, int line) {
    check_exp(type,exp,line);
    this->type = type;
    emit(exp->getCode());
    this->reg = exp->reg;
    this->val = exp->val;
    this->code = exp->code;
    this->type = type;

}



IfStatement::IfStatement(std::string label,Exp * exp, int line) {

    //reg = genBool(exp, line);

}

string genBoolAssign(Exp * exp, int line){
    ExpBool * bool1 = dynamic_cast<ExpBool*>(exp);
    bool1->nextLabel = CodeBuffer::instance().freshLabel();
    CodeBuffer::instance().emit("br label %" + dynamic_cast<ExpBool*>(exp)->nextLabel);
    CodeBuffer::instance().emit(dynamic_cast<ExpBool*>(exp)->falseLabel+":");
    CodeBuffer::instance().emit("br label %" + dynamic_cast<ExpBool*>(exp)->nextLabel);
    CodeBuffer::instance().emit(dynamic_cast<ExpBool*>(exp)->nextLabel+":");
    string reg = Vars::getInstance().freshvar();
    CodeBuffer::instance().emit(reg + " = phi i32 [ 1, %" + bool1->trueLabel + "], [ 0, %" + bool1->falseLabel + "]");
    return reg;
}

string genBool(Exp * exp, int line){
    ExpBool * bool1 = dynamic_cast<ExpBool*>(exp);
    bool1->nextLabel = CodeBuffer::instance().freshLabel();
    CodeBuffer::instance().emit("br label %" + dynamic_cast<ExpBool*>(exp)->nextLabel);
    CodeBuffer::instance().emit(dynamic_cast<ExpBool*>(exp)->falseLabel+":");
    CodeBuffer::instance().emit("br label %" + dynamic_cast<ExpBool*>(exp)->nextLabel);
    CodeBuffer::instance().emit(dynamic_cast<ExpBool*>(exp)->nextLabel+":");
    string reg = Vars::getInstance().freshvar();
    CodeBuffer::instance().emit(reg + " = phi i1 [ 1, %" + bool1->trueLabel + "], [ 0, %" + bool1->falseLabel + "]");
    string trueLabel = CodeBuffer::instance().freshLabel();
    string falseLabel = CodeBuffer::instance().freshLabel();
    CodeBuffer::instance().emit("br i1 " + reg + ", label %" + trueLabel + ", label %" + falseLabel);
    CodeBuffer::instance().emit(trueLabel + ":");
    bool1->falseLabel = falseLabel;
    return reg;
}

IfElseStatement::IfElseStatement(std::string label, Exp *exp, int line) {
    nextLabel = label;
    CodeBuffer::instance().emit("br label %" + nextLabel);
    CodeBuffer::instance().emit(dynamic_cast<ExpBool*>(exp)->falseLabel+":");

}

WhileStatement::WhileStatement(std::string label, Exp *exp, int line) {
    //CodeBuffer::instance().emit("br label %" + dynamic_cast<ExpBool*>(exp)->falseLabel);
   // CodeBuffer::instance().emit(dynamic_cast<ExpBool*>(exp)->falseLabel+":");
}

Vars &Vars::getInstance() {
    {
        static Vars instance;
        return instance;
    }
}

string Vars::freshvar() {

        string reg =  "%t" + to_string(nextReg);
        nextReg++;
        return reg;

}

string Vars::stackInit() {
    stack = freshvar();
    CodeBuffer::instance().emit(stack + " = alloca i32, i32 " + to_string(MAX_STACK_SIZE));
    return stack;
}

