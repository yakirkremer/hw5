//
// Created by yakir on 7/14/2024.
//

#include "types.h"

#include "hw3_output.hpp"
using namespace output;

/*
Node *makeNode(int node_type,const string value,  const string type) {
    switch (node_type) {
        case DECL:
            return new Decl(new Type(type),new Node(value));
        case TYPE:
            return new Type(type);
        case EXP:
            return new Exp(type, value);
        default:
            return nullptr;
    }
}
*/

bool not_num(const string type) {
    if(type != "INT" && type != "BYTE" && type != "num")
        return true;
    return false;
}

Node::Node(const string name) : name(name) {};
Node::Node(const Node* node): name(node->name){};


Exp::Exp(Exp *exp, const std::string type, int yylineno) {
    if(not_num(type) || not_num(exp->type)){
            errorMismatch(yylineno);
            exit(ERROR_EXIT);
    }
    this->type = type;
    val = exp->val;
    is_var = exp->is_var;
    name = exp->name;



}

Exp::Exp(const string type, Node * node, int yylineno) : Node(node),type(type),is_var(false) {
if(type == "BYTE"){
    try {
        val = stoi(node->name);
    }
    catch (exception &e){
        errorByteTooLarge(yylineno,node->name);
        exit(ERROR_EXIT);
    }
    if(val > 255) {
        errorByteTooLarge(yylineno, node->name);
        exit(ERROR_EXIT);
    }

}
else
    val = 0;
}

Exp::Exp(Exp *exp): Node(exp->name), type(exp->type), is_var(exp->is_var), val(exp->val) {}
Exp::Exp(const std::string type, const std::string op, Exp *exp1, Exp *exp2, int yylineno): type(type){

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
    if(op == "relop"){
        if(not_num(exp1->type) || not_num(exp2->type)){
            errorMismatch(yylineno);
            exit(ERROR_EXIT);
        }
    }



}
Exp::Exp(const std::string op, Exp *exp1, Exp *exp2, int yylineno) {
    //Exp MULTIPLICATIVE Exp
        if(not_num(exp1->type) || not_num(exp2->type)){
            errorMismatch(yylineno);
            exit(ERROR_EXIT);
        }
        if(exp1->type == "INT" || exp2->type == "INT"){
            this->type = "INT";
            return;
        }

        else{
            if(op == "add")
                this->val = exp1->val +exp2->val;
            if(op == "mul")
                this->val = exp1->val * exp2->val;
            this->type = "BYTE";
        }

}
Exp::Exp(const std::string name,Node * node, bool is_var, int yylineno): Node(node),is_var(is_var) {
    if(this->is_var){
        if(sym_table_scopes.variable_exists(name) == false){
            errorUndef(yylineno,name);
            exit(ERROR_EXIT);
        }
    }
    Symbol* symbol = sym_table_scopes.get_symbol(name);
    this->type = symbol->get_type();

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

Call::Call(Node *node, Exp *exp, int yylineno) : Node(node){
    if(sym_table_scopes.function_exists(node->name) == false){
        errorUndefFunc(yylineno,node->name);
        exit(ERROR_EXIT);
    }
    this->type = sym_table_scopes.get_symbol(node->name)->get_type();
    if(num_type(exp->type) != sym_table_scopes.get_symbol(node->name)->get_arg_type()){
        errorPrototypeMismatch(yylineno,node->name,sym_table_scopes.get_symbol(node->name)->get_arg_type_for_print());
        exit(ERROR_EXIT);
    }

}

Statement::Statement(Node *type, Node *name, int yylineno) {
    if(sym_table_scopes.symbol_exists(name->name) == true){
        errorDef(yylineno,name->name);
        exit(ERROR_EXIT);
    }
    sym_table_scopes.add_symbol(name->name,type->name,1, "", false);
}

//assign
Statement::Statement( Node *name, Exp *exp, int yylineno, bool declare) {
    if(sym_table_scopes.variable_exists(name->name) == false){
        errorUndef(yylineno,name->name);
        exit(ERROR_EXIT);
    }
    string type_s = sym_table_scopes.get_symbol(name->name)->get_type();
   // if(type_s == "BYTE" && (byte)exp->val >= (byte)255 ){
       // errorByteTooLarge(yylineno,to_string(exp->val));
       // exit(ERROR_EXIT);
    //}
    if(exp->type != type_s){

        if(!not_num(exp->type) && !not_num(type_s) && type_s != "BYTE"){
            return;
        }
        errorMismatch(yylineno);
        exit(ERROR_EXIT);
    }
}
//assign_declare
Statement::Statement(Node *type, Node *name, Exp *exp, int yylineno, bool declare) {
    if(sym_table_scopes.symbol_exists(name->name) == true){
        errorDef(yylineno,name->name);
        exit(ERROR_EXIT);
    }
    string type_s = type->name;
    //if(type_s == "BYTE" && exp->val >= 255 ){
       // errorByteTooLarge(yylineno,to_string(exp->val));
       // exit(ERROR_EXIT);
   // }
    sym_table_scopes.add_symbol(name->name,type->name,1,  "", false);
    if(exp->type != type_s){
        if(!not_num(exp->type) && !not_num(type->name)&& type_s != "BYTE"){
            return;
        }
        errorMismatch(yylineno);
        exit(ERROR_EXIT);
    }


}

Statement::Statement(Exp *exp, const std::string type, int yylineno) {
    if(exp->type != type){
        errorMismatch(yylineno);
        exit(ERROR_EXIT);
    }
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
Exp: LPAREN Exp RPAREN                      {cout<<"the expression value is"<<$1->value<<endl; printProductionRule(19);}
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