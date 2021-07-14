; ModuleID = './sample/error.dc'

@.str = private unnamed_addr constant [1 x i8] zeroinitializer
@.str1 = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare double @printf(i8*, ...)

declare double @__isoc99_scanf(i8*, ...)

declare i32 @print(i8*)

declare i32 @input(i8*)

define double @func() {
entry:
  %a = alloca double
  %var_temp = load double* %a
  store double 2.000000e+00, double* %a
  %var_temp1 = load double* %a
  %var_temp2 = load double* %a
  %add_tmp = fadd double %var_temp2, 3.000000e+00
  store double %add_tmp, double* %a
  %var_temp3 = load double* %a
  ret double %var_temp3
}

define i32 @main() {
entry:
  %call_temp = call double @func()
  %int_tmp = fptosi double %call_temp to i32
  %call_temp1 = call double (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @.str1, i32 0, i32 0), i32 %int_tmp)
  ret i32 0
}
