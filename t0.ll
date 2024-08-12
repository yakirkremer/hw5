
 define i32 @main(){

 label_2:
 %t1 = alloca i32, i32 50
 %t2 = add i32 0, 3
 %t3 = add i32 0, 4
 %t4 = icmp sgt i32 %t2, %t3
 br i1 , label label_3, label label_4

 %t5 = add i32 0, 4
 %t6 = add i32 0, 5
 %t7 = icmp sgt i32 %t5, %t6
 br i1 , label label_5, label label_6

 label_7:
 %t8 = add i32 0, 0
 %t9 = getelemtptr i32, i32* %t0, i32 0
 store i32 %t8, i32* %t9
 label_4:
 ret i32 0
 }

%t2 = add i32 0, 3
%t3 = add i32 0, 4
%t4 = icmp sgt i32 %t2, %t3
br i1 , label label_3, label label_4

%t5 = add i32 0, 4
%t6 = add i32 0, 5
%t7 = icmp sgt i32 %t5, %t6
br i1 , label label_5, label label_6

label_7:
%t8 = add i32 0, 0
%t9 = getelemtptr i32, i32* %t0, i32 0
store i32 %t8, i32* %t9
label_4:
ret i32 0
}

Process finished with exit code 0
