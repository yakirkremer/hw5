#include <iostream>
#include "hw3_output.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

void output::endScope(){
    //cout << "---end scope---" << endl;
}

void output::printID(const string& id, int offset, const string& type) {
    //cout << id << " " << type <<  " " << offset <<  endl;
}

string output::makeFunctionType(const string& argType, const string& retType) {
    
    return "(" + argType + ")" + "->" + retType;
}

const std::string output::rules[] = {
        "Program -> Statements",
        "Statements -> Statement",
        "Statements -> Statements Statement",
        "Statement -> LBRACE Statements RBRACE",
        "Statement -> Type ID SC",
        "Statement -> Type ID ASSIGN Exp SC",
        "Statement -> ID ASSIGN Exp SC",
        "Statement -> Call SC",
        "Statement -> RETURN SC",
        "Statement -> IF LPAREN Exp RPAREN Statement",
        "Statement -> IF LPAREN Exp RPAREN Statement ELSE Statement",
        "Statement -> WHILE LPAREN Exp RPAREN Statement",
        "Statement -> BREAK SC",
        "Statement -> CONTINUE SC",
        "Call -> ID LPAREN Exp RPAREN",
        "Type -> INT",
        "Type -> BYTE",
        "Type -> BOOL",
        "Exp -> LPAREN Exp RPAREN",
        "Exp -> Exp BINOP Exp",
        "Exp -> ID",
        "Exp -> Call",
        "Exp -> NUM",
        "Exp -> NUM B",
        "Exp -> STRING",
        "Exp -> TRUE",
        "Exp -> FALSE",
        "Exp -> NOT Exp",
        "Exp -> Exp AND Exp",
        "Exp -> Exp OR Exp",
        "Exp -> Exp RELOP Exp",
        "Exp -> LPAREN Type RPAREN Exp"
};

void output::printProductionRule(const int ruleno) {
    return;
    std::cout << ruleno << ": " << output::rules[ruleno-1] << "\n";
}

void output::errorLex(const int lineno) {
    std::cout << "line " << lineno << ": lexical error\n";
}

void output::errorSyn(const int lineno) {
    std::cout << "line " << lineno << ": syntax error\n";
}


void output::errorUndef(int lineno, const string& id){
    cout << "line " << lineno << ":" << " variable " << id << " is not defined" << endl;
}

void output::errorDef(int lineno, const string& id){
    cout << "line " << lineno << ":" << " identifier " << id << " is already defined" << endl;
}

void output::errorUndefFunc(int lineno, const string& id) {
    cout << "line " << lineno << ":" << " function " << id << " is not defined" << endl;
}

void output::errorMismatch(int lineno){
    cout << "line " << lineno << ":" << " type mismatch" << endl;
}

void output::errorPrototypeMismatch(int lineno, const string& id, const string &type) {
    cout << "line " << lineno << ": prototype mismatch, function " << id << " expects arguments " << type << endl;
}

void output::errorUnexpectedBreak(int lineno) {
    cout << "line " << lineno << ":" << " unexpected break statement" << endl;
}

void output::errorUnexpectedContinue(int lineno) {
    cout << "line " << lineno << ":" << " unexpected continue statement" << endl;	
}

void output::errorByteTooLarge(int lineno, const string& value) {
    cout << "line " << lineno << ": byte value " << value << " out of range" << endl;
}

