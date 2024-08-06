#include "types.h"
#include "symbol_table.h"
#include "generator.h"
#include "hw3_output.hpp"

#define TRUE "true"
#define FALSE "false"
extern TableStack tables;
extern Generator code_gen;
extern CodeBuffer buffer;
extern int yylineno;

static bool check_types_compatible(string type1, string type2) {
    if (type1 == type2)
        return true;

    if (type1 == "bool" || type2 == "bool" || type1 == "string" || type2 == "string" || type2 == "void")
        return false;
    return true;
}

//************FORMALS******************
FormalDecl::FormalDecl(FormalDecl *formal) : Node(formal->value), type(formal->type) {
    if (DEBUG)
        std::cout << "FormalDecl " << this->type << " " << this->value << std::endl;
}

FormalDecl::FormalDecl(Type *type, Node *node) : Node(*node), type(type->type) {
    if (DEBUG)
        std::cout << "FormalDecl " << this->type << " " << this->value << std::endl;
}

FormalsList::FormalsList(Node *node) : Node(), formals_list() {
    if (DEBUG)
        std::cout << "FormalsList " << std::endl;
    formals_list.insert(formals_list.begin(), new FormalDecl(dynamic_cast<FormalDecl *>(node)));
}

FormalsList::FormalsList(Node *node, FormalsList *formals_list) : Node(), formals_list() {
    if (DEBUG)
        std::cout << "FormalsList " << std::endl;
    for (auto it = formals_list->formals_list.begin(); it != formals_list->formals_list.end(); ++it) {
        FormalDecl *decl = new FormalDecl(*it);
        this->formals_list.push_back(decl);
    }
    this->formals_list.insert(this->formals_list.begin(), dynamic_cast<FormalDecl *>(node));
//    delete[] formals_list;
}

Formals::Formals() : Node(), formals_list() {
    if (DEBUG)
        std::cout << "Formals " << std::endl;

}

Formals::Formals(FormalsList *formals_list) : Node(), formals_list() {
    if (DEBUG)
        std::cout << "Formals " << std::endl;
    for (auto it = formals_list->formals_list.begin(); it != formals_list->formals_list.end(); ++it) {
        FormalDecl *decl = new FormalDecl(*it);
        this->formals_list.push_back(decl);
    }

//    delete[] formals_list;
}

//************FUNCDECL*****************
// FuncDecl -> RetType ID LPAREN Formals RPAREN LBRACE OS Statements ES RBRACE
FuncDecl::FuncDecl(RetType *return_type, Node *id, Formals *params) {
    if (DEBUG)
        std::cout << "FuncDecl " + return_type->type + " " + id->value << std::endl;
    if (tables.symbol_exists(id->value)) {
        output::errorDef(yylineno, id->value);
        exit(0);
    }
    vector <string> function_param_types = vector<string>();
    for (int i = 0; i < params->formals_list.size(); ++i) {
        auto it = params->formals_list[i];
        function_param_types.push_back((*it).type);
    }
    tables.add_symbol(id->value, return_type->type, true, function_param_types);
    tables.push_scope(false, return_type->type);
    int offset = -1;
    for (int i = 0; i < params->formals_list.size(); ++i) {
        auto it = params->formals_list[i];
        if (tables.symbol_exists(it->value)) {
            output::errorDef(yylineno, it->value);
            exit(0);
        }
        tables.add_function_symbol(it->value, it->type, offset);
        offset -= 1;
    }

    code_gen.function_declaration_code(id, params, return_type);
    tables.current_scope()->rbp = code_gen.allocate_function_stack();
}

//************STATEMENT****************
// Statement -> BREAK SC / CONTINUE SC
Statement::Statement(Node *node) {
    int address = buffer.emit("br label @");
    if (node->value == "break") {
        if (!tables.check_loop()) {
            output::errorUnexpectedBreak(yylineno);
            exit(0);
        }
        this->break_list = buffer.makelist(pair<int, BranchLabelIndex>(address, FIRST));
//        std::cout << "Break list created\n";
    } else if (node->value == "continue") {
        if (!tables.check_loop()) {
            output::errorUnexpectedContinue(yylineno);
            exit(0);
        }
        this->cont_list = buffer.makelist(pair<int, BranchLabelIndex>(address, FIRST));
    }
//    std::cout << "Statement b " << break_list.size() << " c " << cont_list.size() << "\n" ;
}

void Statement::merge_lists_statements(Node *node) {
    Statements *s = dynamic_cast<Statements *>(node);
    break_list = buffer.merge(break_list, s->break_list);
    cont_list = buffer.merge(cont_list, s->cont_list);
}

Statement::Statement(Exp *exp, bool is_return) : Node() {
    if (DEBUG)
        std::cout << "Statement Expression is_return:" << is_return << std::endl;
    SymbolTable *scope = tables.current_scope();
    string *return_type = scope->return_type;
//    std::cout << "Return type " + *return_type + "\n";

    if (*return_type != "" && *return_type != exp->type) {
        if (*return_type != "int" || exp->type != "byte") {
            output::errorMismatch(yylineno);
            exit(0);
        }

    }
    string rbp = tables.current_scope()->rbp;
    string reg;
    if (exp->is_var) {
        Symbol *symbol = tables.get_symbol(exp->value);
        if (symbol->is_function) {
            output::errorUndef(yylineno, symbol->name);
            exit(0);
        }
        if (symbol->offset >= 0) {
            reg = code_gen.generate_load_var(rbp, symbol->offset);
        } else {
            reg = "%" + std::to_string(((-1) * symbol->offset -1));
        }
        if(exp->type == "bool"){
            exp->reg = reg;
            Exp* new_exp = code_gen.bool_exp(exp);
            code_gen.return_value(new_exp->type, new_exp->reg);
            delete new_exp;
        } else {
            code_gen.return_code(*return_type, reg);
        }
    } else {
//        std::cout << "Exp type: " << exp->type + " " + exp->reg + " " + exp->value + "\n";
        if (exp->type == "void") {
            string empty = "";
            code_gen.return_code(empty, reg);
        } else if(exp->type == "bool") {
            Exp* new_exp = code_gen.bool_exp(exp);
            code_gen.return_value(new_exp->type, new_exp->reg);
            delete new_exp;
        } else {
            if(!exp->value.empty()){
                Symbol* symbol = tables.get_symbol(exp->value);
                if(symbol){
                    // Function call
                    code_gen.return_value(exp->type, exp->reg);
                } else {
                    code_gen.return_value(exp->type, exp->value);
                }
            } else {
                code_gen.return_value(exp->type, exp->reg);
            }
        }
    }
}

// Statement -> Type ID SC
Statement::Statement(Type *type, Node *id) : Node() {
    if (DEBUG)
        std::cout << "Statement Type ID: " << type->type << " " << id->value << std::endl;
    if (tables.symbol_exists(id->value)) {
        output::errorDef(yylineno, id->value);
        exit(0);
    }
    tables.add_symbol(id->value, type->type, false);
    Symbol* symbol = tables.get_symbol(id->value);
    value = type->value;
    Exp *temp = new Exp();
    temp->reg = code_gen.allocate_register();
    temp->type = value;
    if (value == "bool") {
        temp->value = "false";
        code_gen.ruleBool(temp);
    } else {
        temp->value = "0";
        code_gen.ruleNum(temp);
        code_gen.assign_code(temp, symbol->offset, symbol->type == "bool");
    }
    delete temp;
}

// Statement -> Type ID ASSIGN Exp SC or Statement -> AUTO ID ASSIGN Exp SC
Statement::Statement(Type *type, Node *id, Exp *exp) : Node() {
    if (DEBUG)
        std::cout << "Statement -> Type ID ASSIGN Exp SC\n";

    if (tables.symbol_exists(id->value)) {
        output::errorDef(yylineno, id->value);
        exit(0);
    }
    string var_type;
    if (type) {
        if (!check_types_compatible(type->type, exp->type)) {
            output::errorMismatch(yylineno);
            exit(0);
        }
        if (type->type == "byte" && exp->type == "int") {
            output::errorMismatch(yylineno);
            exit(0);
        }
        var_type = type->type;
        tables.add_symbol(id->value, type->type, false);
    } else {
        if (exp->type == "void" || exp->type == "string") {
            output::errorMismatch(yylineno);
            exit(0);
        }
        tables.add_symbol(id->value, exp->type, false);
        var_type = exp->type;
    }

    Symbol *symbol = tables.get_symbol(id->value);
    code_gen.assign_code(exp, symbol->offset, var_type == "bool");
}

// Statement -> ID ASSIGN Exp SC
Statement::Statement(Node *id, Exp *exp) : Node(), cont_list(), break_list() {
    if (!tables.symbol_exists(id->value)) {
        output::errorUndef(yylineno, id->value);
        exit(0);
    }
    Symbol *symbol = tables.get_symbol(id->value);
    if (symbol->is_function || !check_types_compatible(symbol->type, exp->type)) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if (symbol->type == "byte" && exp->type == "int") {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if(symbol->offset >= 0 ){
        code_gen.assign_code(exp, symbol->offset, symbol->type == "bool");
    } else {

    }
}

// Statement -> Call SC
Statement::Statement(Call *call) : Node() {
    if (DEBUG)
        std::cout << "Statement Call " << call->value << std::endl;
    if (!tables.symbol_exists(call->value)) {
        output::errorUndefFunc(yylineno, call->value);
        exit(0);
    }
    Symbol *symbol = tables.get_symbol(call->value);
    if (!symbol->is_function) {
        output::errorUndefFunc(yylineno, call->value);
        exit(0);
    }
    Exp exp = Exp();
    exp.type = call->type;
    exp.true_list = call->true_list;
    exp.false_list = call->false_list;
    code_gen.bool_exp(&exp);
}

Statement::Statement(Statement* statement, Exp *exp, Label *label) {
    if (DEBUG)
        std::cout << "Exp (bool)\n";
    if (exp->type != "bool") {
        output::errorMismatch(yylineno);
        exit(0);
    }
    break_list = buffer.merge(break_list, statement->break_list);
    cont_list = buffer.merge(cont_list, statement->cont_list);

    buffer.bpatch(exp->true_list, label->value);
    string new_label = buffer.genLabel();
    code_gen.label_block_code(new_label);
    buffer.bpatch(exp->false_list, new_label);
    buffer.bpatch(exp->next_list, new_label);
}

Statement::Statement(Statement* statement1, Statement* statement2, Exp *exp, Label *true_label, Label *false_label) {
    if (DEBUG)
        std::cout << "Exp (bool) 2\n";
    if (exp->type != "bool") {
        output::errorMismatch(yylineno);
        exit(0);
    }
    cont_list =  buffer.merge(statement1->cont_list, statement2->cont_list);
    break_list = buffer.merge(statement1->break_list, statement2->break_list);

    buffer.bpatch(exp->true_list, true_label->value);
    buffer.bpatch(exp->false_list, false_label->value);
    string new_label = buffer.genLabel();
    code_gen.label_block_code(new_label);
    buffer.bpatch(exp->next_list, new_label);
}

Statement::Statement(const string& name, Exp *exp, Label *exp_label, Label *true_label, Statement *statement) {
    buffer.emit_uncond_jump(exp_label->value);
    string new_label = code_gen.allocate_label("label_");
    code_gen.label_block_code(new_label);
//    string emitted_true_label = true_label->value[0] == '%' ? true_label->value.substr(1, true_label->value.size()) : true_label->value;
//    string emitted_new_label = new_label[0] == '%' ? new_label.substr(1, new_label.size()) : new_label;
//    string emitted_exp_label = exp_label->value[0] == '%' ? exp_label->value.substr(1, exp_label->value.size()) : exp_label->value;

    buffer.bpatch(exp->true_list, true_label->value);
    buffer.bpatch(exp->false_list, new_label);
    buffer.bpatch(exp->next_list, new_label);
    buffer.bpatch(statement->break_list, new_label);
    buffer.bpatch(statement->cont_list, exp_label->value);
}

//****************TYPE************************
Type::Type(
        const string type) : Node(), type(type) {

}

RetType::RetType(
        const string type) : Node(), type(type) {}

// ***************EXP******************
// Exp -> LPAREN Exp RPAREN
Exp::Exp(Exp *exp) : Node(exp->value), type(exp->type), reg(exp->reg), false_list(exp->false_list),
true_list(exp->true_list) , next_list(exp->next_list){
}

// Exp -> CONST(bool, num, byte, string)
Exp::Exp(Node *terminal, string type) : Node(terminal->value), type(type) {
    if (DEBUG)
        std::cout << "Exp Node+string " << type << " " << terminal->value << std::endl;
    if (type == "byte") {
        int value = stoi(terminal->value);
        if (value > 255) {
            output::errorByteTooLarge(yylineno, terminal->value);
            exit(0);
        }
    }
//    value = terminal->value;
    if (type == "int" || type == "byte") {
        reg = code_gen.allocate_register();
        code_gen.ruleNum(this);
    } else if (type == "string") {
        code_gen.ruleStr(this);
    } else {
        code_gen.ruleBool(this);
    }

}

//Exp -> ID, Call
Exp::Exp(bool is_var, Node *terminal) : Node(), is_var(is_var) {
    if (DEBUG)
        std::cout << "Exp -> ID, Call " << terminal->value << " is var: " << is_var << std::endl;
    if (is_var && !tables.symbol_exists(terminal->value)) {
        output::errorUndef(yylineno, terminal->value);
        output::errorUndef(yylineno, terminal->value);
        exit(0);
    }
    Symbol *symbol = tables.get_symbol(terminal->value);
    value = terminal->value;
    type = symbol->type;
    this->is_var = is_var;
    if (!is_var) {
        Call* func = dynamic_cast<Call *>(terminal);
        reg = func->reg;
        true_list = func->true_list;
        false_list = func->false_list;
        return;
    }
    code_gen.ruleID(this);
}

Exp::Exp(Node *terminal1,
         Node *terminal2,
         const string op,
         const string type,
         const string label = "") {
    Exp *exp1 = dynamic_cast<Exp *>(terminal1);
    Exp *exp2 = dynamic_cast<Exp *>(terminal2);

    if (DEBUG)
        std::cout << "Exp op " << exp1->type << " " << exp1->value << " " << exp2->type << " " << exp2->value
                  << std::endl;
    if (exp1->is_var && !tables.symbol_exists(exp1->value)) {
        output::errorUndef(yylineno, terminal1->value);
        exit(0);
    }

    if (exp2->is_var && !tables.symbol_exists(exp2->value)) {
        output::errorUndef(yylineno, terminal2->value);
        exit(0);
    }

    if (!check_types_compatible(exp1->type, exp2->type)) {
        output::errorMismatch(yylineno);
        exit(0);
    }

    if (type == "bool") {
        if (exp1->type != "bool" || exp2->type != "bool") {
            output::errorMismatch(yylineno);
            exit(0);
        }

        this->type = "bool";
        code_gen.bool_eval_code(this, dynamic_cast<Exp *>(exp1), dynamic_cast<Exp *>(exp2), op, label);
    } else if (type == "int") {

        // TODO: same condition twice
        if ((exp1->type != "int" && exp1->type != "byte") || (exp1->type != "int" && exp1->type != "byte")) {
            output::errorMismatch(yylineno);
            exit(0);
        }

        if (exp1->type == "int" || exp2->type == "int") {
            this->type = "int";
        } else {
            this->type = "byte";
        }
        this->value = "";
        code_gen.binop_code(this, exp1, exp2, op);

        if (DEBUG)
            std::cout << "op return type is " << this->type << " reg " << this->reg << " type " << this->type << "\n";

    } else if (type == "relop") {
        if (!check_types_compatible(exp1->type, exp2->type)) {
            output::errorMismatch(yylineno);
            exit(0);
        }
        if ((exp1->type != "int" && exp1->type != "byte") || (exp1->type != "int" && exp1->type != "byte")) {
            output::errorMismatch(yylineno);
            exit(0);
        }
        this->type = "bool";
        code_gen.relop_code(this, exp1, exp2, op);
    }
}

// Exp -> LPAREN Type RPAREN Exp
Exp::Exp(Node *exp, Node *type) {
    Exp *converted_exp = dynamic_cast<Exp *>(exp);
    Type *converted_type = dynamic_cast<Type *>(type);

    if (!check_types_compatible(converted_exp->type, converted_type->type)) {
        output::errorMismatch(yylineno);
        exit(0);
    }

    this->value = converted_exp->value;
    this->type = converted_type->type;
    this->reg = converted_exp->reg;
}

//*******************EXPLIST************************

// ExpList -> Exp
ExpList::ExpList(Node *exp) : Node(), expressions() {
    if (DEBUG)
        std::cout << "ExpList -> Exp: " << exp->value << "\n";

    Exp *expression = dynamic_cast<Exp *>(exp);
//    Exp *exp_to_insert = code_gen.bool_exp(expression);
//
    expressions.push_back(expression);
}

//// ExpList -> Exp, ExpList
//ExpList::ExpList(Node *exp_list, Node *exp)i : Node(), expressions() {
//    if (DEBUG)
//        std::cout << "ExpList -> Exp,ExpList" << "\n";
//    expressions.push_back(dynamic_cast<Exp *>(exp));
//    vector<Exp *> expressions_list = (dynamic_cast<ExpList *>(exp_list))->expressions;
//    for (int i = 0; i < expressions_list.size(); ++i) {
//        expressions.push_back(new Exp(expressions_list[i]));
//
//    }
//
//}

//*******************CALL*********************

// Call -> ID LPAREN RPAREN
Call::Call(Node *terminal) : Node() {
    if (DEBUG)
        std::cout << "Call " << terminal->value << std::endl;

    string name = terminal->value;
    if (!tables.symbol_exists(name)) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }

    Symbol *symbol = tables.get_symbol(name);
    if (!symbol->is_function) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }

    if (symbol->params.size() > 0) {
        vector <string> converted_params;
        for (int i = 0; i < symbol->params.size(); ++i) {
            converted_params.push_back(convert_to_upper_case(symbol->params[i]));
        }
        output::errorPrototypeMismatch(yylineno, name, converted_params);
        exit(0);
    }

    type = symbol->type;
    value = symbol->name;
    ExpList empty_exp = ExpList();
    code_gen.function_code(this, &empty_exp);
}

// Call -> ID LPAREN ExpList RPAREN
Call::Call(Node *terminal, Node *exp_list) : Node() {
    if (DEBUG)
        std::cout << "Call " << terminal->value << std::endl;
    ExpList *expressions_list = dynamic_cast<ExpList *>(exp_list);
    string name = terminal->value;
    if (!tables.symbol_exists(name)) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }
    Symbol *symbol = tables.get_symbol(name);
    if (!symbol->is_function) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }
    if (symbol->params.size() != expressions_list->expressions.size()) {
        vector <string> converted_params;
        for (int i = 0; i < symbol->params.size(); ++i) {
            converted_params.push_back(convert_to_upper_case(symbol->params[i]));
        }
        output::errorPrototypeMismatch(yylineno, name, converted_params);
        exit(0);
    }
    for (int i = 0; i < symbol->params.size(); i++) {
        if (symbol->params[i] != expressions_list->expressions[i]->type) {
            if (symbol->params[i] != "int" || expressions_list->expressions[i]->type != "byte") {
                vector <string> converted_params = vector<string>();
                for (int j = 0; j < symbol->params.size(); ++j) {
                    converted_params.push_back(convert_to_upper_case(symbol->params[j]));
                }
                output::errorPrototypeMismatch(yylineno, name, converted_params);
                exit(0);
            }
        }
    }
    type = symbol->type;
    value = symbol->name;
    code_gen.function_code(this, expressions_list);
}

Program::Program() {

}

Label::Label() : Node("") {
    value = code_gen.allocate_label("label_");
    if (DEBUG)
        std::cout << "Label: " << value << "\n";
    code_gen.label_block_code(value);
}

void check_bool(Node *node) {
    Exp *exp = dynamic_cast<Exp *>(node);
    if (exp->type != "bool") {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

Statements::Statements(Statement *statement) {
//    std::cout << "Statements1 statement sizes b " << statement->break_list.size() << " c " << statement->cont_list.size() << "\n";
    break_list = buffer.merge(break_list, statement->break_list);
    cont_list = buffer.merge(cont_list, statement->cont_list);
//    std::cout << "Statements1 b " << break_list.size() << " c " << cont_list.size() << "\n" ;
    delete statement;
}

Statements::Statements(Statements *statements, Statement *statement) : Node(), cont_list(), break_list() {
//    std::cout << "Statements2 statement sizes b " << statement->break_list.size() << " c " << statement->cont_list.size() << "\n";
//    std::cout << "Statements2 statement sizes b " << statements->break_list.size() << " c " << statements->cont_list.size() << "\n";
    break_list = buffer.merge(statements->break_list, statement->break_list);
    cont_list = buffer.merge(statements->cont_list, statement->cont_list);
//    std::cout << "Statements2 b " << break_list.size() << " c " << cont_list.size() << "\n" ;
    delete statements;
    delete statement;
}
