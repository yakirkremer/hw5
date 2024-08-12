@.DIV_BY_ZERO_ERROR = internal constant [23 x i8] c"Error division by zero\00"
define void @check_division(i32) {
%valid = icmp eq i32 %0, 0
br i1 %valid, label %ILLEGAL, label %LEGAL
ILLEGAL:
call void @print(i8* getelementptr([23 x i8], [23 x i8]* @.DIV_BY_ZERO_ERROR, i32 0, i32 0))
call void @exit(i32 0)
ret void
LEGAL:
ret void
}
@.intFormat = internal constant [4 x i8] c"%d\0A\00"
@.DIVIDE_BY_ZERO.str = internal constant [23 x i8] c"Error division by zero\00"
declare i32 @printf(i8*, ...)
declare void @exit(i32)
@.int_specifier = constant [4 x i8] c"%d\0A\00"
@.str_specifier = constant [4 x i8] c"%s\0A\00"
define void @print(i8*){
call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %0)
ret void
}
define void @printi(i32){
%format_ptr = getelementptr [4 x i8], [4 x i8]* @.intFormat, i32 0, i32 0
call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.intFormat, i32 0, i32 0), i32 %0)
ret void
}
define i32 @main(){
label_2:
%t0 = alloca i32, i32 50
%t1 = add i32 0, 6
%t2 = add i32 0, 4
%t3 = sdiv i32 %t1, %t2
%t4 = getelementptr i32, i32* %t0, i32 0
store i32 %t3, i32* %t4
ret i32 0
}
