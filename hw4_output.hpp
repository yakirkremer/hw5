#ifndef _236360_3_
#define _236360_3_

#include <vector>
#include <string>
using namespace std;

#define LEX_ERROR 1
#define SYNTAX_ERROR 2
#define UNDEFINED_ERROR 3
#define DEFINED_ERROR 4
#define UNDEFINED_FUNC_ERROR 5
#define MISMATCH_ERROR 6
#define PROTOTYPE_MISMATCH_ERROR 7
#define UNEXPECTED_BREAK_ERROR 8
#define UNEXPECTED_CONTINUE_ERROR 9
#define BYTE_TOO_LARGE_ERROR 10

namespace output
{
    void endScope();
    void printID(const string &id, int offset, const string &type);
    string makeFunctionType(const string& argType, const string& retType);


    extern const std::string rules[];
    void printProductionRule(const int ruleno);

    void errorLex(int lineno);
    void errorSyn(int lineno);
    void errorUndef(int lineno, const string &id);
    void errorDef(int lineno, const string &id);
    void errorUndefFunc(int lineno, const string &id);
    void errorMismatch(int lineno);
    void errorPrototypeMismatch(int lineno, const string &id, const string &type);
    void errorUnexpectedBreak(int lineno);
    void errorUnexpectedContinue(int lineno);
    void errorByteTooLarge(int lineno, const string &value);
}


#endif