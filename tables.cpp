//
// Created by yakir on 7/14/2024.
//

#include "tables.h"
using namespace output;






string convert_num(string s){
    if (s=="num")
        return "INT";
    return s;
}

void print_table(SymbolTable *table) {
    return;
    for (int i = 0; i < table->get_size(); i++) {
            cout << "Symbol " << table->get_symbol(i)->get_name()  << std::endl;
    }
}

int SymbolTable::add_symbol(string name, string type,int size,string pointer, bool is_function = false, string arg_type = "") {
    if(symbol_exists(name)){
        return false;
    }

    symbols.push_back(new Symbol(name, type, max_offset, is_function ,pointer ,arg_type));
    max_offset += size;

    return max_offset-size;
}

bool SymbolTable::symbol_exists(const std::string &name) {

    for(int i = 0; i < symbols.size(); i++){
       if(symbols[i]->get_name() == name) {
           return true;
       }
    }
    return false;
}

Symbol * SymbolTable::get_symbol(const string & name){
    for(int i = symbols.size()-1; i >= 0; i--) {
        if (symbols[i]->get_name() == name)
            return symbols[i];
    }
    return nullptr;
}

bool Scopes::symbol_exists(const string &name) {
    for(int i = table_scopes.size()-1; i >= 0; i--){
        if(table_scopes[i]->symbol_exists(name)){
            return true;
        }
    }

    return false;
}

bool SymbolTable::function_exists(const std::string &name) {
    for(int i = 0; i < symbols.size(); i++){
        if(symbols[i]->get_name() == name && symbols[i]->is_function){
            return true;
        }
    }
    return false;
}

bool SymbolTable::variable_exists(const std::string &name) {
    for(int i = 0; i < symbols.size(); i++){
       // cout<<symbols[i]->get_name();
        if(symbols[i]->get_name() == name && !symbols[i]->is_function){
            return true;
        }
    }
    return false;
}

int Scopes::add_symbol(const string &name, const string &type, int size,string pointer, bool is_function,string arg_type) {
    if(scope->symbol_exists(name)){
        return false;
    }
    return scope->add_symbol(name, type, size, pointer, is_function,arg_type);
    //scope->max_offset += size;
    return true;
}

Symbol *Scopes::get_symbol(const string &name) {
    for(int i = table_scopes.size()-1; i >= 0; i--){
        Symbol *symbol = table_scopes[i]->get_symbol(name);
        if(symbol != nullptr)
            return symbol;
    }
    return nullptr;
}

Scopes::Scopes(): table_scopes(), offsets(), scope(nullptr) {
    offsets.push_back(0);
    table_scopes.push_back(new SymbolTable(offsets.back()));
    scope = table_scopes.back();
    //this->table_scopes.push_back(new SymbolTable(0));
    //this->scope = table_scopes.back();
    //this->offsets.push_back(0);
}

void print_scopes(vector<SymbolTable *> scopes) {
    for (int i = 0; i < scopes.size(); i++) {
        cout << "Scope " << i << std::endl;
        print_table(scopes[i]);
    }
}

void Scopes::open_scope(bool new_loop) {
    offsets.push_back(scope->max_offset);
    table_scopes.push_back(new SymbolTable(offsets.back()));
    bool is_loop_open = scope->is_loop;
    scope = table_scopes.back();
    if(is_loop_open || new_loop){
        scope->is_loop = true;
    }
}

void Scopes::close_scope() {
    endScope();
    SymbolTable *last = table_scopes.back();
    for (int i = 0; i < scope->symbols.size(); ++i) {
        if(scope->symbols[i]->is_function)
            printID(scope->symbols[i]->name, scope->symbols[i]->offset,makeFunctionType(convert_num(scope->symbols[i]->arg_type),scope->symbols[i]->type));
        else
            printID(scope->symbols[i]->name, scope->symbols[i]->offset, scope->symbols[i]->type);
    }
    table_scopes.pop_back();
    delete last;
    scope = table_scopes.back();
    if(table_scopes.size() != 0){
        scope->max_offset = offsets.back();
        offsets.pop_back();
    }

}

bool Scopes::in_loop() {
    return scope->is_loop;
}

bool Scopes::function_exists(const std::string &name) {
    for(int i = table_scopes.size()-1; i >= 0; i--){
        if(table_scopes[i]->function_exists(name)){
            return true;
        }
    }

    return false;
}

bool Scopes::variable_exists(const std::string &name) {
    for(int i = table_scopes.size()-1; i >= 0; i--){
        if(table_scopes[i]->variable_exists(name)){
            return true;
        }
    }

    return false;
}


