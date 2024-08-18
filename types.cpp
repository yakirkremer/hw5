//
// Created by yakir on 8/9/2024.
//
/*
#include "types.h"
#include "CodeGen.h"

bool not_num(const string type);

bool check_exp(std::string newType,string op,Terminal *exp1, Terminal *exp2, int line){

    if(newType == "BOOL"){
        if(op == "and" || op == "or" || op == "not"){
            if(dynamic_cast<Exp*>(exp1)->exp_type != "BOOL" || dynamic_cast<Exp*>(exp2)->exp_type != "BOOL"){
                errorMismatch(line);
                exit(ERROR_EXIT);
            }
        }
        if((op == "relop") ) {
            if (not_num(dynamic_cast<Exp*>(exp1)->exp_type) || not_num(dynamic_cast<Exp*>(exp2)->exp_type)) {
                errorMismatch(line);
                exit(ERROR_EXIT);
            }

        }
    }
    if(op == "add" || op == "mul"){
        if(not_num(dynamic_cast<Exp*>(exp1)->exp_type) || not_num(dynamic_cast<Exp*>(exp2)->exp_type)){
            errorMismatch(line);
            exit(ERROR_EXIT);
        }
    }

    if(newType == "BYTE"){
        int val;
        try {

            val = stoi(exp1->val);
        }
        catch (exception &e){
            errorByteTooLarge(line,exp1->val);
            exit(ERROR_EXIT);
        }
        if(val > 255) {
            errorByteTooLarge(line, exp1->val);
            exit(ERROR_EXIT);
        }

    }

    return true;
}

Label::Label() {
    label = CodeBuffer::instance().freshLabel();
    code = label + ":\n";
}

Label::Label(string label) : label(label) {code = label + ":\n";}

Terminal::Terminal(const string terminalVal) : val(terminalVal) {}

Terminal::Terminal() {
    val = "";
}



//Not Exp
//Exp AND Exp
//Exp OR Exp
//Exp RELOP Exp
//Exp EQUALITY Exp
//Exp: Exp MULTIPLICATIVE Exp
//Exp: Exp ADDITIVE Exp
Exp::Exp(Terminal *exp1, Terminal *exp2, std::string newType, std::string op,Terminal * opTerminal , int line):exp_type(newType) {



    //and, or, not
    if(newType == "BOOL"){
        branch = true;
        CodeGen::getInstance().genBranch(dynamic_cast<Exp*>(exp1), true);
        CodeGen::getInstance().genBranch(dynamic_cast<Exp*>(exp2), true);
        check_exp(newType,op,exp1,exp2, line);
        this->exp_type =  newType;
        if(op == "and"){
            trueLabel = dynamic_cast<Exp*>(exp2)->trueLabel;
            falseLabel = dynamic_cast<Exp*>(exp1)->falseLabel;
            dynamic_cast<Exp*>(exp1)->emitBranch(dynamic_cast<Exp*>(exp1)->trueLabel->label + ":");
            dynamic_cast<Exp*>(exp2)->emitBranch(dynamic_cast<Exp*>(exp2)->falseLabel->label + ":");
            dynamic_cast<Exp*>(exp2)->emitBranch("br label %" + falseLabel->label);
            dynamic_cast<Exp*>(exp1)->emitBranch(dynamic_cast<Exp*>(exp2)->getBranchCode());
            emitBranch(dynamic_cast<Exp*>(exp1)->getBranchCode());
            //emit(dynamic_cast<Exp*>(exp2)->getBranchCode());
        }
        if(op == "or"){
            trueLabel = dynamic_cast<Exp*>(exp1)->trueLabel;
            falseLabel = dynamic_cast<Exp*>(exp2)->falseLabel;
            dynamic_cast<Exp*>(exp1)->emitBranch(dynamic_cast<Exp*>(exp1)->falseLabel->label + ":");
            dynamic_cast<Exp*>(exp2)->emitBranch(dynamic_cast<Exp*>(exp2)->trueLabel->label + ":");
            dynamic_cast<Exp*>(exp2)->emitBranch("br label %" + trueLabel->label);
            dynamic_cast<Exp*>(exp1)->emitBranch(dynamic_cast<Exp*>(exp2)->getBranchCode());
            emitBranch(dynamic_cast<Exp*>(exp1)->getBranchCode());
            //emitBranch(dynamic_cast<Exp*>(exp2)->getBranchCode());
        }
        if (op == "not"){
            trueLabel = dynamic_cast<Exp*>(exp1)->falseLabel;
            falseLabel = dynamic_cast<Exp*>(exp1)->trueLabel;
            emitBranch(dynamic_cast<Exp*>(exp1)->getBranchCode());
        }
    }


    if(op == "relop"){
        check_exp(newType,op,exp1,exp2, line);

        //trueLabel = new Label();
        //falseLabel = new Label();

        reg = CodeGen::getInstance().genBool(dynamic_cast<Exp *>(exp1), dynamic_cast<Exp *>(exp2), opTerminal->val, this);
        emitBranch("br i1 " + reg + ", label %" + trueLabel->label + ", label %" + falseLabel->label);
        CodeGen::getInstance().genBranch(this, true);
        //cout<<code<<endl;
    }
    //relop

    if(op == "add" || op == "mul") {
        check_exp(newType,op,exp1,exp2, line);

        if (dynamic_cast<Exp *>(exp1)->exp_type != "INT" && dynamic_cast<Exp *>(exp2)->exp_type != "INT") {
            this->exp_type = "BYTE";
        }
        else
            this->exp_type = "INT";

        reg = CodeGen::getInstance().genExp(dynamic_cast<Exp *>(exp1), dynamic_cast<Exp *>(exp2), this->exp_type,opTerminal);
        //reg = CodeGen::getInstance().genTrunc(reg, this->exp_type);

    }

    if(op == "cast"){
        if(not_num(dynamic_cast<Exp*>(exp1)->exp_type) || not_num(newType)){
            errorMismatch(line);
            exit(ERROR_EXIT);
        }
        reg = dynamic_cast<Exp*>(exp1)->reg;

        this->exp_type = newType;
        val = exp1->val;

    }


    //ID


}

//Exp: LPAREN Exp RPAREN
Exp::Exp(Exp * exp):Terminal(exp->val) {


    trueLabel = exp->trueLabel;
    falseLabel = exp->falseLabel;
    reg = exp->reg;
    code = exp->code;
    exp_type = exp->exp_type;
    exp->val = val;
    branch = exp->branch;


}



void printExp(Exp *exp){
    cout<<exp->val<<endl;
    cout<<exp->exp_type<<endl;
    cout<<exp->reg<<endl;
    cout<<exp->code<<endl;
    cout<<exp->trueLabel->label<<endl;
    cout<<exp->falseLabel->label<<endl;
}

//Exp: ID
//Exp: Call
//Exp: NUM
//Exp: NUM B
//Exp: STRING
//Exp: TRUE
//Exp: FALSE
//Exp: LPAREN Type RPAREN Exp

Exp::Exp(Terminal * exp1,string newType, int line):Terminal(exp1->val),exp_type(newType) {

    if(newType == "BYTE")
        check_exp(newType,"INIT",exp1,exp1, line);

    if(newType == "ID"){

        if(sym_table_scopes.variable_exists(exp1->val) == false){
            errorUndef(line,exp1->val);
            exit(ERROR_EXIT);
        }
        this->exp_type = sym_table_scopes.get_symbol(exp1->val)->get_type();
        string pointer = sym_table_scopes.get_symbol(exp1->val)->pointer;
        this->val = exp1->val;
        reg = CodeGen::getInstance().loadID(pointer, this->exp_type);

            //CodeGen::getInstance().genBranch(this, true);
        //}

        //printExp(this);
        return;

    }

    if(newType == "Call"){
        this->exp_type = dynamic_cast<Call*>(exp1)->func_type;
        reg = dynamic_cast<Call*>(exp1)->reg;
        return;

    }

    CodeGen::getInstance().getInstance().genExp(this);



}

void Exp::emit(std::string add_code) {
    code = code +"\n" + add_code;
    branchCode = branchCode +"\n" + add_code;
}

void Exp::emitBranch(std::string code1) {
    branchCode = branchCode +"\n" + code1;
}

void Exp::emitCode(std::string code1) {
    code = code +"\n" + code1;
}



ExpBool::ExpBool(Terminal *exp, int line): Exp(dynamic_cast<Exp*>(exp)) {


    //printExp(dynamic_cast<Exp*>(exp));
    if(dynamic_cast<Exp*>(exp)->exp_type != "BOOL"){
        errorMismatch(line);
        exit(ERROR_EXIT);
    }

    CodeGen::getInstance().genBranch(dynamic_cast<Exp*>(this), true);
    CodeBuffer::instance().emit(dynamic_cast<Exp*>(this)->getBranchCode());



    CodeBuffer::instance().emit(dynamic_cast<Exp*>(this)->trueLabel->code);

}

Call::Call(Terminal *terminal, Terminal *exp, int line) {
    if(sym_table_scopes.function_exists(terminal->val) == false){
        errorUndefFunc(line,terminal->val);
        exit(ERROR_EXIT);
    }
    this->func_type = sym_table_scopes.get_symbol(terminal->val)->get_type();
    if(num_type(dynamic_cast<Exp*>(exp)->exp_type) != sym_table_scopes.get_symbol(terminal->val)->get_arg_type()){
        errorPrototypeMismatch(line,terminal->val,sym_table_scopes.get_symbol(terminal->val)->get_arg_type_for_print());
        exit(ERROR_EXIT);
    }
    if(terminal->val != "readi") {
        string func_arg_type = sym_table_scopes.get_symbol(terminal->val)->get_arg_type();
        string type = dynamic_cast<Exp *>(exp)->exp_type;
        dynamic_cast<Exp *>(exp)->reg = CodeGen::getInstance().cast(dynamic_cast<Exp *>(exp)->reg, type, func_arg_type);
            CodeGen::getInstance().genCall(sym_table_scopes.get_symbol(terminal->val)->name, dynamic_cast<Exp *>(exp)->reg);
    }

    else {

        reg = CodeGen::getInstance().genReadi(dynamic_cast<Exp*>(exp)->reg);
    }




}


Statement::Statement(int mode, Terminal *id, Terminal *type, Terminal *exp, int line) {


    switch (mode) {
        //ID ASSIGN Exp SC

        case ASSIGN_ONLY: {
            CodeBuffer::instance().emit(dynamic_cast<Exp*>(exp)->getCode());
            if (sym_table_scopes.variable_exists(id->val) == false) {
                errorUndef(line, id->val);
                exit(ERROR_EXIT);
            }
            string type_s = sym_table_scopes.get_symbol(id->val)->get_type();
            if (dynamic_cast<Exp *>(exp)->exp_type != type_s) {
                if (!not_num(dynamic_cast<Exp *>(exp)->exp_type) && !not_num(type_s) && type_s != "BYTE") {
                    return;
                }
                errorMismatch(line);
                exit(ERROR_EXIT);
            }
            string exp_reg = dynamic_cast<Exp *>(exp)->reg;
            string exp_type = dynamic_cast<Exp *>(exp)->exp_type;

            CodeGen::getInstance().assignID(dynamic_cast<Exp *>(exp)->reg, sym_table_scopes.get_symbol(id->val)->pointer);

            break;
        }
        //Type ID SC
        case DEFINE_ONLY: {

            if (sym_table_scopes.symbol_exists(id->val) == true) {
                errorDef(line, id->val);
                exit(ERROR_EXIT);
            }

            int offset = sym_table_scopes.scope->max_offset;
            string pointer = CodeGen::getInstance().genID(CodeGen::getInstance().genExp(type->val), offset);
            sym_table_scopes.add_symbol(id->val, type->val, 1,pointer , false, "");

            break;
        }
        //Type ID ASSIGN Exp SC
        case DEFINE_ASSIGN: {
            CodeBuffer::instance().emit(dynamic_cast<Exp*>(exp)->getCode());
            if (sym_table_scopes.symbol_exists(id->val) == true) {
                errorDef(line, id->val);
                exit(ERROR_EXIT);
            }


            if (dynamic_cast<Exp *>(exp)->exp_type != type->val) {

                if (!(!not_num(dynamic_cast<Exp *>(exp)->exp_type) && !not_num(type->val) && type->val != "BYTE")) {
                    errorMismatch(line);
                    exit(ERROR_EXIT);

                }
            }

            int offset = sym_table_scopes.scope->max_offset;
            string exp_type = dynamic_cast<Exp *>(exp)->exp_type;
            string exp_reg = dynamic_cast<Exp *>(exp)->reg;


            string pointer = CodeGen::getInstance().genID(dynamic_cast<Exp*>(exp)->reg, offset);
            sym_table_scopes.add_symbol(id->val, type->val, 1,pointer , false,"");
            break;
        }

    }
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
 */