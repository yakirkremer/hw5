@t14 = constant [13 x i8] c"you are good\00"
@t15 = constant [18 x i8] c"you code is wrong\00"
@.DIV_BY_ZERO_ERROR = internal constant [23 x i8] c"Error division by zero\00"
@.intFormat = internal constant [4 x i8] c"%d\0A\00"
@.DIVIDE_BY_ZERO.str = internal constant [23 x i8] c"Error division by zero\00"
declare i32 @printf(i8*, ...)
declare void @exit(i32)
declare i32 @scanf(i8*, ...)
@.int_specifier_scan = constant [3 x i8] c"%d\00"
@.int_specifier = constant [4 x i8] c"%d\0A\00"
@.str_specifier = constant [4 x i8] c"%s\0A\00"
define i32 @readi(i32) {
%ret_val = alloca i32
%spec_ptr = getelementptr [3 x i8], [3 x i8]* @.int_specifier_scan, i32 0, i32 0
call i32 (i8*, ...) @scanf(i8* %spec_ptr, i32* %ret_val)
%val = load i32, i32* %ret_val
ret i32 %val
}
define void @print(i8*){
%spec_ptr = getelementptr [4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0
call i32 (i8*, ...) @printf(i8* %spec_ptr, i8* %0)
ret void
}
define void @printi(i32){
%spec_ptr = getelementptr [4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0
call i32 (i8*, ...) @printf(i8* %spec_ptr, i32 %0)
ret void
}
define i32 @main(){
label_2:
%t0 = alloca i32, i32 50


%t1 = add i32 0, 255
%t2 = getelementptr i32, i32* %t0, i32 0
store i32 %t1, i32* %t2



%t3 = load i32, i32* %t2

%t4 = add i32 0, 2
%t5 = add i32 %t3, %t4
%t6 = trunc i32 %t5 to  i8
%t7 = zext i8 %t6 to i32
store i32 %t7, i32* %t2


%t8 = add i32 0, 1
%t9 = getelementptr i32, i32* %t0, i32 1
store i32 %t8, i32* %t9


%t10 = load i32, i32* %t2

%t11 = load i32, i32* %t9
%t12 = icmp eq i32 %t10, %t11
br i1 %t12, label %label_3, label %label_4
label_3:
br label %label_5
label_4:
br label %label_5
label_5:
%t13 = phi i1 [ 1, %label_3], [ 0, %label_4]
br i1 %t13, label %label_6, label %label_7
label_6:


%t14.ptr = getelementptr [13 x i8], [13 x i8]* @t14, i32 0, i32 0
call void @print( i8* %t14.ptr)
br label %label_8
label_7:


%t15.ptr = getelementptr [18 x i8], [18 x i8]* @t15, i32 0, i32 0
call void @print( i8* %t15.ptr)
br label %label_8
label_8:
ret i32 0
divByZero:
call void @print(i8* getelementptr([23 x i8], [23 x i8]* @.DIV_BY_ZERO_ERROR, i32 0, i32 0))
call void @exit(i32 0)
ret i32 -1
}
