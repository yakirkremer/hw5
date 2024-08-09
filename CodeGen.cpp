//
// Created by yakir on 8/6/2024.
//

#include "CodeGen.h"


CodeGen::CodeGen() : nextReg(0) {}

CodeGen &CodeGen::getInstance() {
    static CodeGen instance;
    return instance;
}

string CodeGen::freshVar() {
    string reg =  "%t" + to_string(nextReg);
    nextReg++;
    return reg;
}

void CodeGen::genBranch(const std::string var, const std::string &label) {
    CodeBuffer::instance().emit("br i1 " + var + ", label %" + label + ", label %" + CodeBuffer::instance().freshLabel());
}

