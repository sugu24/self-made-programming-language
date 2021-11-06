; ModuleID = 'sample/test.gd'

@i = common global double 0.000000e+00
@.str = private unnamed_addr constant [1 x i8] zeroinitializer
@.str1 = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare double @printf(i8*, ...)

declare double @__isoc99_scanf(i8*, ...)

declare i32 @print(i8*)

declare i32 @input(i8*)

define i32 @main() {
entry:
  %var_temp = load double* @i
  store double 2.000000e+00, double* @i
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %var_temp1 = load double* @i
  %cmp = fcmp ole double %var_temp1, 5.000000e+00
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %var_temp2 = load double* @i
  %int_tmp = fptosi double %var_temp2 to i32
  %call_temp = call double (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @.str1, i32 0, i32 0), i32 %int_tmp)
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %var_temp3 = load double* @i
  %add_tmp = fadd double %var_temp3, 1.000000e+00
  store double %add_tmp, double* @i
  br label %for.cond

for.end:                                          ; preds = %for.cond
  ret i32 0
}
