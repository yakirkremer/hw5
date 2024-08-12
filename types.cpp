//
// Created by yakir on 8/9/2024.
//

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

Label::Label(string label) : label(label) {}

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
        check_exp(newType,op,exp1,exp2, line);
        this->exp_type =  newType;
        if(op == "and"){
            curLabel = new Label();
            trueLabel = dynamic_cast<Exp*>(exp2)->trueLabel;
            falseLabel = dynamic_cast<Exp*>(exp1)->falseLabel;
            dynamic_cast<Exp*>(exp1)->emit(dynamic_cast<Exp*>(exp1)->trueLabel->label + ":");
            dynamic_cast<Exp*>(exp2)->emit(dynamic_cast<Exp*>(exp2)->falseLabel->label + ":");
            dynamic_cast<Exp*>(exp2)->emit("br label %" + falseLabel->label);
            dynamic_cast<Exp*>(exp1)->emit(dynamic_cast<Exp*>(exp2)->getCode());
            emit(dynamic_cast<Exp*>(exp1)->getCode());
            //emit(dynamic_cast<Exp*>(exp2)->getCode());
        }
        if(op == "or"){
            curLabel = new Label();

            trueLabel = dynamic_cast<Exp*>(exp1)->trueLabel;
            falseLabel = dynamic_cast<Exp*>(exp2)->falseLabel;
            dynamic_cast<Exp*>(exp1)->emit(dynamic_cast<Exp*>(exp1)->falseLabel->label + ":");
            dynamic_cast<Exp*>(exp2)->emit(dynamic_cast<Exp*>(exp2)->trueLabel->label + ":");
            dynamic_cast<Exp*>(exp2)->emit("br label %" + trueLabel->label);
            dynamic_cast<Exp*>(exp1)->emit(dynamic_cast<Exp*>(exp2)->getCode());
            emit(dynamic_cast<Exp*>(exp1)->getCode());
            //emit(dynamic_cast<Exp*>(exp2)->getCode());
        }
    }

    if(op == "relop"){
        check_exp(newType,op,exp1,exp2, line);

        reg = CodeGen::getInstance().genBool(dynamic_cast<Exp *>(exp1), dynamic_cast<Exp *>(exp2), opTerminal->val, this);
        CodeGen::getInstance().genBranch(this);
       //cout<<curLabel->code;
    }
    //relop

    if(op == "add" || op == "mul") {
        check_exp(newType,op,exp1,exp2, line);

        if (dynamic_cast<Exp *>(exp1)->exp_type != "INT" && dynamic_cast<Exp *>(exp2)->exp_type != "INT") {
            this->exp_type = "BYTE";
        }
        reg = CodeGen::getInstance().genExp(dynamic_cast<Exp *>(exp1), dynamic_cast<Exp *>(exp2), opTerminal);
    }

    if(op == "cast"){
        if(not_num(dynamic_cast<Exp*>(exp1)->exp_type) || not_num(newType)){
            errorMismatch(line);
            exit(ERROR_EXIT);
        }
        this->exp_type = newType;
        val = exp1->val;
    }


    //ID


}

//Exp: LPAREN Exp RPAREN
Exp::Exp(Exp * exp):Terminal(exp->val), code(exp->code),exp_type(exp->exp_type),trueLabel(exp->trueLabel), falseLabel(exp->falseLabel), curLabel(exp->curLabel) {}

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
        reg = CodeGen::getInstance().loadID(pointer);
        return;

    }

    if(newType == "Call"){
        this->exp_type = dynamic_cast<Call*>(exp1)->func_type;
    }
    CodeGen::getInstance().getInstance().genExp(this);


}

void Exp::emit(std::string add_code) {
    code = code +"\n" + add_code;
}



ExpBool::ExpBool(Terminal *exp, int line): Exp(dynamic_cast<Exp*>(exp)) {
    if(dynamic_cast<Exp*>(exp)->exp_type != "BOOL"){
        errorMismatch(line);
        exit(ERROR_EXIT);
    }

    CodeBuffer::instance().emit(dynamic_cast<Exp*>(exp)->getCode());
    CodeBuffer::instance().emit(dynamic_cast<Exp*>(exp)->trueLabel->code);
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

        CodeGen::getInstance().genCall(sym_table_scopes.get_symbol(terminal->val)->name,dynamic_cast<Exp*>(exp)->reg);



}


Statement::Statement(int mode, Terminal *id, Terminal *type, Terminal *exp, int line) {
    switch (mode) {
        //ID ASSIGN Exp SC
        case ASSIGN_ONLY: {
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
            int offset = sym_table_scopes.get_symbol(id->val)->offset;
            CodeGen::getInstance().genID(exp_reg, offset);
            break;
        }
        //Type ID SC
        case DEFINE_ONLY: {

            if (sym_table_scopes.symbol_exists(id->val) == true) {
                errorDef(line, id->val);
                exit(ERROR_EXIT);
            }
            int offset = sym_table_scopes.scope->max_offset;
            string pointer = CodeGen::getInstance().genID(CodeGen::getInstance().genNum(), offset);
            sym_table_scopes.add_symbol(id->val, type->val, 1,pointer , false, "");

            break;
        }
        //Type ID ASSIGN Exp SC
        case DEFINE_ASSIGN: {
            if (sym_table_scopes.symbol_exists(id->val) == true) {
                errorDef(line, id->val);
                exit(ERROR_EXIT);
            }


            if (dynamic_cast<Exp *>(exp)->exp_type != type->val) {

                if (!not_num(dynamic_cast<Exp *>(exp)->exp_type) && !not_num(type->val) && type->val != "BYTE") {
                    return;
                }
                errorMismatch(line);
                exit(ERROR_EXIT);
            }

            int offset = sym_table_scopes.scope->max_offset;
            string pointer = CodeGen::getInstance().genID(dynamic_cast<Exp*>(exp)->reg, offset);
            sym_table_scopes.add_symbol(id->val, type->val, 1,pointer , false,"");
            break;
        }

    }
}

Statement::Statement(Terminal *expBool) {
    trueLabel = dynamic_cast<Exp*>(expBool)->trueLabel;
    falseLabel = dynamic_cast<Exp*>(expBool)->falseLabel;
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