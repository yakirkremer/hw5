//
// Created by yakir on 7/14/2024.
//

#ifndef HW3_TABLES_H
#define HW3_TABLES_H

#include <string>
#include <vector>
#include <iostream>
#include "hw3_output.hpp"

#define CHECK false


using namespace std;

class Symbol {
public:
    string name;
    string type;
    string arg_type;
    int offset;
    bool is_function;


    Symbol(const string name, const string type, int offset, bool is_function,const string arg_type =  "") : name(name), type(type), offset(offset),is_function(is_function), arg_type(arg_type) {}
    string get_name() const { return name; }
    string get_type() const { return type; }
    string get_arg_type() const {

        return arg_type;
    }
    string get_arg_type_for_print() const {
        if(arg_type == "num")
            return "INT";
        return arg_type;
    }


};

class SymbolTable {
public:
    vector<Symbol *> symbols;
    int offset;
    int max_offset;
    bool is_loop;

    Symbol* get_symbol(int index) const { return symbols[index]; }
    int get_size() const { return symbols.size(); }
    SymbolTable(int offset)
            :max_offset(offset),offset(offset) {
        this->symbols = vector<Symbol *>();
    }

    bool add_symbol(string name, string type, int size, bool is_function, string arg_type);

    bool symbol_exists(const string &name);

    Symbol *get_symbol(const string &name);

    bool function_exists(const string &name);
    bool variable_exists(const string &name);

    ~SymbolTable() {
        for (auto it = symbols.begin(); it != symbols.end(); it++)
            delete (*it);
    }
};

class Scopes {
public:
    vector<SymbolTable *> table_scopes;
    vector<int> offsets;
    SymbolTable * scope;

    Scopes();

    bool add_symbol(const string &name, const string &type, int size,string arg_type, bool is_function);
    bool symbol_exists(const string &name);
    bool function_exists(const string &name);
    bool variable_exists(const string &name);
    Symbol* get_symbol(const string &name);
    void open_scope(bool new_loop = false);
    void close_scope();
    bool in_loop();


};



#endif //HW3_TABLES_H
