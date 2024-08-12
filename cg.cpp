#include "cg.hpp"
#include <vector>
#include <iostream>
#include <sstream>
using namespace std;

void print_to_file(const std::string& str, FILE *file_ptr) {

    if (file_ptr != NULL) {
        fprintf(file_ptr, "%s\n", str.c_str());

    } else {
        std::cerr << "Error opening file." << std::endl;
    }
}

CodeBuffer::CodeBuffer() : buffer(), globalDefs() {}

CodeBuffer &CodeBuffer::instance() {
	static CodeBuffer inst;//only instance
	return inst;
}

string CodeBuffer::freshLabel(){
	std::stringstream label;
	label << "label_" << ++labels_num;
	return label.str();
}

string CodeBuffer::currentLabel(){
    std::stringstream label;
    label << "label_" << labels_num;
    return label.str();
}

int CodeBuffer::emit(const string &s){
    buffer.push_back(s);
	return buffer.size() - 1;
}

void CodeBuffer::printCodeBuffer(){
    FILE *file_ptr = fopen("demo.ll", "w");
    for (vector<string>::const_iterator it = globalDefs.begin(); it != globalDefs.end(); ++it)
    {
        //cout << *it << endl;
        print_to_file(*it, file_ptr);
    }
	for (std::vector<string>::const_iterator it = buffer.begin(); it != buffer.end(); ++it)
	{

		//cout << *it << endl;
        print_to_file(*it, file_ptr);
    }
    fclose(file_ptr);

}

int CodeBuffer::emitOpen(){
    return emit("define i32 @main(){");
}

int CodeBuffer::emitClose(){
    return emit("ret i32 0\n}");
}

// ******** Methods to handle the global section ********** //
void CodeBuffer::emitGlobal(const std::string& dataLine) 
{
	globalDefs.push_back(dataLine);
}

void CodeBuffer::printGlobalBuffer()
{
	for (vector<string>::const_iterator it = globalDefs.begin(); it != globalDefs.end(); ++it)
	{
		//cout << *it << endl;
	}
}

void CodeBuffer::emitInit()
{

    emit("@.DIV_BY_ZERO_ERROR = internal constant [23 x i8] c\"Error division by zero\\00\"");

    // Division by zero check function
    emit("define void @check_division(i32) {");
    emit("%valid = icmp eq i32 %0, 0");
    emit("br i1 %valid, label %ILLEGAL, label %LEGAL");
    emit("ILLEGAL:");
    emit("call void @print(i8* getelementptr([23 x i8], [23 x i8]* @.DIV_BY_ZERO_ERROR, i32 0, i32 0))");
    emit("call void @exit(i32 0)");
    emit("ret void");
    emit("LEGAL:");
    emit("ret void");
    emit("}");
    emit("@.intFormat = internal constant [4 x i8] c\"%d\\0A\\00\"");
    emit("@.DIVIDE_BY_ZERO.str = internal constant [23 x i8] c\"Error division by zero\\00\"");

    emit("declare i32 @printf(i8*, ...)");
    emit("declare void @exit(i32)");

    emit("@.int_specifier = constant [4 x i8] c\"%d\\0A\\00\"");
    emit("@.str_specifier = constant [4 x i8] c\"%s\\0A\\00\"");
    emit("define void @print(i8*){");
    emit("call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %0)");
    emit("ret void");
    emit("}");

    emit("define void @printi(i32){");
    emit("%format_ptr = getelementptr [4 x i8], [4 x i8]* @.intFormat, i32 0, i32 0");
    emit("call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.intFormat, i32 0, i32 0), i32 %0)");
    emit("ret void");
    emit("}");
}
