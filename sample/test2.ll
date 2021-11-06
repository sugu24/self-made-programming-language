; ModuleID = 'sample/test2.gd'

@a = common global double 0.000000e+00
@.str = private unnamed_addr constant [1 x i8] zeroinitializer
@.str1 = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@.str2 = private unnamed_addr constant [1 x i8] zeroinitializer
@.str3 = private unnamed_addr constant [6 x i8] c"%.3f\0A\00"
@.str4 = private unnamed_addr constant [1 x i8] zeroinitializer
@.str5 = private unnamed_addr constant [7 x i8] c"%2.2f\0A\00"
@.str6 = private unnamed_addr constant [1 x i8] zeroinitializer
@.str7 = private unnamed_addr constant [9 x i8] c"aa\5Cnaa\5Cn\00"
@.str8 = private unnamed_addr constant [9 x i8] c"aa\5Cnaa\5Cn\00"

declare double @printf(i8*, ...)

declare double @__isoc99_scanf(i8*, ...)

declare i32 @print(i8*)

declare i32 @input(i8*)

define i32 @main() {
entry:
  %var_temp = load double* @a
  store double 1.000000e+03, double* @a
  %var_temp1 = load double* @a
  %int_tmp = fptosi double %var_temp1 to i32
  %call_temp = call double (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @.str1, i32 0, i32 0), i32 %int_tmp)
  %var_temp2 = load double* @a
  %call_temp3 = call double (i8*, ...)* @printf(i8* getelementptr inbounds ([6 x i8]* @.str3, i32 0, i32 0), double %var_temp2)
  %var_temp4 = load double* @a
  %call_temp5 = call double (i8*, ...)* @printf(i8* getelementptr inbounds ([7 x i8]* @.str5, i32 0, i32 0), double %var_temp4)
  %call_temp6 = call double (i8*, ...)* @printf(i8* getelementptr inbounds ([9 x i8]* @.str8, i32 0, i32 0))
  ret i32 0
}
