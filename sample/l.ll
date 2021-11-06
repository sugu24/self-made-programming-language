; ModuleID = 'sample/l.gd'

@a = common global double 0.000000e+00
@.str = private unnamed_addr constant [1 x i8] zeroinitializer
@.str1 = private unnamed_addr constant [3 x i8] c"1 \00"
@.str2 = private unnamed_addr constant [6 x i8] c"1 %d\0A\00"
@.str3 = private unnamed_addr constant [1 x i8] zeroinitializer
@.str4 = private unnamed_addr constant [3 x i8] c"2 \00"
@.str5 = private unnamed_addr constant [6 x i8] c"2 %d\0A\00"

declare double @printf(i8*, ...)

declare double @__isoc99_scanf(i8*, ...)

declare i32 @print(i8*)

declare i32 @input(i8*)

define i32 @main() {
entry:
  %var_temp = load double* @a
  store double 2.000000e+00, double* @a
  %var_temp1 = load double* @a
  %cmp = fcmp oeq double %var_temp1, 1.000000e+00
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %var_temp2 = load double* @a
  %int_tmp = fptosi double %var_temp2 to i32
  %call_temp = call double (i8*, ...)* @printf(i8* getelementptr inbounds ([6 x i8]* @.str2, i32 0, i32 0), i32 %int_tmp)
  br label %if.end

if.end:                                           ; preds = %entry, %if.then
  %var_temp3 = load double* @a
  %cmp4 = fcmp oeq double %var_temp3, 2.000000e+00
  br i1 %cmp4, label %if.then5, label %if.end9

if.then5:                                         ; preds = %if.end
  %var_temp6 = load double* @a
  %int_tmp7 = fptosi double %var_temp6 to i32
  %call_temp8 = call double (i8*, ...)* @printf(i8* getelementptr inbounds ([6 x i8]* @.str5, i32 0, i32 0), i32 %int_tmp7)
  br label %if.end9

if.end9:                                          ; preds = %if.end, %if.then5
  ret i32 0
}
