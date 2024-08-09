//
// Created by yakir on 7/14/2024.
//

#include "types.h"

#include "hw3_output.hpp"
using namespace output;

/*
Node *makeNode(int terminal_type,const string numValue,  const string type) {
    switch (terminal_type) {
        case DECL:
            return new Decl(new Type(type),new Node(numValue));
        case TYPE:
            return new Type(type);
        case EXP:
            return new Exp(type, numValue);
        default:
            return nullptr;
    }
}
*/

string relopTranslate(string op){
    if(op == "<")
        return "slt";
    if(op == ">")
        return "sgt";
    if(op == "<=")
        return "sle";
    if(op == ">=")
        return "sge";
    if(op == "==")
        return "eq";
    if(op == "!=")
        return "ne";
    return "";

}

bool not_num(const string type) {
    if(type != "INT" && type != "BYTE" && type != "num")
        return true;
    return false;
}

Node::Node(const string terminalVal) : terminalVal(terminalVal) {}
Node::Node(const Node* terminal): terminalVal(terminal->terminalVal){}


Exp::Exp(Exp *exp, const std::string type, int yylineno):code("") {
    if(not_num(type) || not_num(exp->type)){
            errorMismatch(yylineno);
            exit(ERROR_EXIT);
    }
    this->type = type;
    numVal = exp->numVal;
    is_var = exp->is_var;
    terminalVal = exp->terminalVal;
    var = exp->var;
    code = exp->code;

}

Exp::Exp(const string type, Node * terminal, int yylineno) : Node(terminal),type(type),is_var(false),code("") {
    if(type == "BOOL"){
        size = 1;
        if(terminal->terminalVal == "true")
            terminalVal = "1";
        else
            terminalVal = "0";
    }
    if(type == "BYTE"){
        size = 8;
        try {
            numVal = stoi(terminal->terminalVal);
        }
        catch (exception &e){
            errorByteTooLarge(yylineno,terminal->terminalVal);
            exit(ERROR_EXIT);
        }
        if(numVal > 255) {
            errorByteTooLarge(yylineno, terminal->terminalVal);
            exit(ERROR_EXIT);
        }

    }
    else
        numVal = 0;
    regInit("add", "0" ,terminalVal);

}



int max(int a, int b){
    if(a > b)
        return a;
    return b;
}

Exp::Exp(const std::string op, Exp *exp1, Exp *exp2, int yylineno):code("") {
    //Exp MULTIPLICATIVE Exp


        if(not_num(exp1->type) || not_num(exp2->type)){
            errorMismatch(yylineno);
            exit(ERROR_EXIT);
        }
        if(exp1->type == "INT" || exp2->type == "INT"){
            this->type = "INT";
        }

        else{
            if(op == "add")
                this->numVal = exp1->numVal +exp2->numVal;
            if(op == "mul")
                this->numVal = exp1->numVal * exp2->numVal;
            this->type = "BYTE";
        }
    regInit(op, exp1->var ,exp2->var);

}
Exp::Exp(const std::string terminalVal,Node * terminal, bool is_var, int yylineno): Node(terminal),is_var(is_var),code("") {
    var = terminalVal;
    if(this->is_var){
        if(sym_table_scopes.variable_exists(terminalVal) == false){
            errorUndef(yylineno,terminalVal);
            exit(ERROR_EXIT);
        }
    }
    Symbol* symbol = sym_table_scopes.get_symbol(terminalVal);
    this->type = symbol->get_type();

    }

void Exp::regInit(string op, string opr1, string opr2) {
    var = CodeGen::getInstance().freshVar();
    code+=var + " = " + op + " i"+ to_string(size) + " " + opr1 + ", " + opr2;
    //CodeBuffer::instance().emit();
    sym_table_scopes.scope->add_symbol(var,type,1,  false,"");
}
Exp::Exp(Exp *exp) {
    this->type = exp->type;
    this->numVal = exp->numVal;
    this->is_var = exp->is_var;
    this->terminalVal = exp->terminalVal;
    this->var = exp->var;
    this->code = exp->code;
    this->size = exp->size;
}

ExpBool::ExpBool(const std::string op, Exp *exp1, Exp *exp2, int yylineno): Exp(op ,exp1,exp2,yylineno), trueLabel(CodeBuffer::instance().freshLabel()), falseLabel(CodeBuffer::instance().freshLabel()) {
    this->size = 1;

    //NOT Exp
    if(op == "not" && exp1->type != "BOOL"){
        errorMismatch(yylineno);
        exit(ERROR_EXIT);
    }
    //Exp AND Exp
    if(op == "and" || op == "or" ){
        if(exp1->type != "BOOL" || exp2->type != "BOOL"){
            errorMismatch(yylineno);
            exit(ERROR_EXIT);
        }
    }
    //Exp RELOP Exp Exp EQUALITY Exp
    if (op == "relop") {
        if (not_num(exp1->type) || not_num(exp2->type)) {
            errorMismatch(yylineno);
            exit(ERROR_EXIT);
        }
        var = CodeGen::getInstance().freshVar();
        code += exp1->code;
        code += exp2->code;
        code += var + " = icmp "  + " i" + to_string(exp1->size) + " " +
                exp1->var + " " + " " + exp2->var + "\n";
        //CodeBuffer::instance().emit();
        //CodeBuffer::instance().emit(CodeBuffer::instance().freshLabel()+":");
    }
}

ExpBool::ExpBool(Exp *exp, int yylineno): Exp(exp) {
    if(exp->type != "BOOL"){
        errorMismatch(yylineno);
        exit(ERROR_EXIT);
    }
}

string num_type(const string type){
    if(type == "INT" || type == "BYTE")
        return "num";
    return type;
}

Call::Call(Node *terminal, Exp *exp, int yylineno) : Node(terminal){
    if(sym_table_scopes.function_exists(terminal->terminalVal) == false){
        errorUndefFunc(yylineno,terminal->terminalVal);
        exit(ERROR_EXIT);
    }
    this->type = sym_table_scopes.get_symbol(terminal->terminalVal)->get_type();
    if(num_type(exp->type) != sym_table_scopes.get_symbol(terminal->terminalVal)->get_arg_type()){
        errorPrototypeMismatch(yylineno,terminal->terminalVal,sym_table_scopes.get_symbol(terminal->terminalVal)->get_arg_type_for_print());
        exit(ERROR_EXIT);
    }

}

Statement::Statement(Node *type, Node *terminalVal, int yylineno) {
    if(sym_table_scopes.symbol_exists(terminalVal->terminalVal) == true){
        errorDef(yylineno,terminalVal->terminalVal);
        exit(ERROR_EXIT);
    }
    sym_table_scopes.add_symbol(terminalVal->terminalVal,type->terminalVal,1, "", false);
    CodeBuffer::instance().emit(code);
}

//assign
Statement::Statement( Node *terminalVal, Exp *exp, int yylineno, bool declare) {
    if(sym_table_scopes.variable_exists(terminalVal->terminalVal) == false){
        errorUndef(yylineno,terminalVal->terminalVal);
        exit(ERROR_EXIT);
    }
    string type_s = sym_table_scopes.get_symbol(terminalVal->terminalVal)->get_type();
   // if(type_s == "BYTE" && (byte)exp->numVal >= (byte)255 ){
       // errorByteTooLarge(yylineno,to_string(exp->numVal));
       // exit(ERROR_EXIT);
    //}
    if(exp->type != type_s){

        if(!not_num(exp->type) && !not_num(type_s) && type_s != "BYTE"){
            return;
        }
        errorMismatch(yylineno);
        exit(ERROR_EXIT);
    }
        CodeBuffer::instance().emit(code);

}
//assign_declare
Statement::Statement(Node *type, Node *terminalVal, Exp *exp, int yylineno, bool declare) {
    if(sym_table_scopes.symbol_exists(terminalVal->terminalVal) == true){
        errorDef(yylineno,terminalVal->terminalVal);
        exit(ERROR_EXIT);
    }
    string type_s = type->terminalVal;
    //if(type_s == "BYTE" && exp->numVal >= 255 ){
       // errorByteTooLarge(yylineno,to_string(exp->numVal));
       // exit(ERROR_EXIT);
   // }
    sym_table_scopes.add_symbol(terminalVal->terminalVal,type->terminalVal,1,  "", false);
    if(exp->type != type_s){
        if(!not_num(exp->type) && !not_num(type->terminalVal)&& type_s != "BYTE"){
            return;
        }
        errorMismatch(yylineno);
        exit(ERROR_EXIT);
    }
        CodeBuffer::instance().emit(code);


}

Statement::Statement(Exp *exp, const std::string type, int yylineno){
    if(exp->type != type){
        errorMismatch(yylineno);
        exit(ERROR_EXIT);
    }
        CodeBuffer::instance().emit(code);
}

Statement(Node * terminalVal, int yylineno):Node(terminalVal) {
    if(sym_table_scopes.variable_exists(terminalVal->terminalVal) == false){
        errorUndef(yylineno,terminalVal->terminalVal);
        exit(ERROR_EXIT);
    }
    if(sym_table_scopes.get_symbol(terminalVal->terminalVal)->get_type() != "BOOL"){
        errorMismatch(yylineno);
        exit(ERROR_EXIT);
    }
    CodeBuffer::instance().emit(code);

}




/*
Program: Statements                         {printProductionRule(1);}
Statements: Statement                       {printProductionRule(2);}
Statements: Statements Statement            {printProductionRule(3);}
Statement: LBRACE Statements RBRACE         {printProductionRule(4);}
Statement: Type ID SC                       {printProductionRule(5);}
Statement: Type ID ASSIGN Exp SC            {printProductionRule(6);}
Statement: ID ASSIGN Exp SC                 {printProductionRule(7);}
Statement: Call SC                          {printProductionRule(8);}
Statement: RETURN SC                        {printProductionRule(9);}
Statement: IF LPAREN Exp RPAREN Statement   {printProductionRule(10);}
Statement: IF LPAREN Exp RPAREN Statement ELSE Statement    {printProductionRule(11);}
Statement: WHILE LPAREN Exp RPAREN Statement    {printProductionRule(12);}
Statement: BREAK SC                         {printProductionRule(13);}
Statement: CONTINUE SC                      {printProductionRule(14);}
Call: ID LPAREN Exp RPAREN                  {printProductionRule(15);}
Type: INT                                   {printProductionRule(16);}
Type: BYTE                                  {printProductionRule(17);}
Type: BOOL                                  {printProductionRule(18);}
Exp: LPAREN Exp RPAREN                      {cout<<"the expression numValue is"<<$1->numValue<<"\n"; printProductionRule(19);}
Exp: Exp MULTIPLICATIVE Exp                 {printProductionRule(20);}
Exp: Exp ADDITIVE Exp                       {printProductionRule(20);}
Exp: ID                                     {printProductionRule(21);}
Exp: Call                                   {printProductionRule(22);}
Exp: NUM                                    {$$ = new Exp($1, "num");}
Exp: NUM B                                  {printProductionRule(24);}
Exp: STRING                                 {printProductionRule(25);}
Exp: TRUE                                   {printProductionRule(26);}
Exp: FALSE                                  {printProductionRule(27);}
Exp: NOT Exp                                {printProductionRule(28);}
Exp: Exp AND Exp                            {printProductionRule(29);}
Exp: Exp OR Exp                             {printProductionRule(30);}
Exp: Exp RELOP Exp                          {printProductionRule(31);}
Exp: Exp EQUALITY Exp                       {printProductionRule(31);}
Exp: LPAREN Type RPAREN Exp                 {printProductionRule(32);}
 */