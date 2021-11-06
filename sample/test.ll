; ModuleID = 'sample/test2.gd'

@.str = private unnamed_addr constant [1 x i8] zeroinitializer
@.str1 = private unnamed_addr constant [9 x i8] c"%%%s.0f \00"
@.str2 = private unnamed_addr constant [1 x i8] zeroinitializer
@.str3 = private unnamed_addr constant [7 x i8] c"%%.5f \00"
@.str4 = private unnamed_addr constant [9 x i8] c"%%%s.0f \00"
@.str5 = private unnamed_addr constant [1 x i8] zeroinitializer
@.str6 = private unnamed_addr constant [7 x i8] c"%%.5f \00"
@.str7 = private unnamed_addr constant [9 x i8] c"%%%s.0f \00"
@.str8 = private unnamed_addr constant [1 x i8] zeroinitializer
@.str9 = private unnamed_addr constant [7 x i8] c"%%.5f \00"
@.str10 = private unnamed_addr constant [10 x i8] c"%%%s.%sf \00"
@.str11 = private unnamed_addr constant [3 x i8] c"10\00"
@.str12 = private unnamed_addr constant [2 x i8] c"3\00"
@.str13 = private unnamed_addr constant [4 x i8] c"aaa\00"
@.str14 = private unnamed_addr constant [4 x i8] c"%s \00"
@.str15 = private unnamed_addr constant [2 x i8] c"\0A\00"
@.str16 = private unnamed_addr constant [1 x i8] zeroinitializer
@.str17 = private unnamed_addr constant [9 x i8] c"%%%s.0f \00"
@.str18 = private unnamed_addr constant [1 x i8] zeroinitializer
@.str19 = private unnamed_addr constant [7 x i8] c"%%.5f \00"
@.str20 = private unnamed_addr constant [9 x i8] c"%%%s.0f \00"
@.str21 = private unnamed_addr constant [1 x i8] zeroinitializer
@.str22 = private unnamed_addr constant [7 x i8] c"%%.5f \00"
@.str23 = private unnamed_addr constant [9 x i8] c"%%%s.0f \00"
@.str24 = private unnamed_addr constant [1 x i8] zeroinitializer
@.str25 = private unnamed_addr constant [7 x i8] c"%%.5f \00"
@.str26 = private unnamed_addr constant [10 x i8] c"%%%s.%sf \00"
@.str27 = private unnamed_addr constant [2 x i8] c"9\00"
@.str28 = private unnamed_addr constant [2 x i8] c"3\00"
@.str29 = private unnamed_addr constant [4 x i8] c"bbb\00"
@.str30 = private unnamed_addr constant [4 x i8] c"%s \00"
@.str31 = private unnamed_addr constant [2 x i8] c"\0A\00"
@.str32 = private unnamed_addr constant [1 x i8] zeroinitializer
@.str33 = private unnamed_addr constant [18 x i8] c"i\E3\81\AF5\E3\81\AA\E3\82\93\E3\81\A7\E3\81\99\00"
@.str34 = private unnamed_addr constant [4 x i8] c"%s \00"
@.str35 = private unnamed_addr constant [2 x i8] c"\0A\00"
@.str36 = private unnamed_addr constant [1 x i8] zeroinitializer
@.str37 = private unnamed_addr constant [27 x i8] c"i\E3\81\AF5\E3\81\A7\E3\81\AF\E3\81\AA\E3\81\84\E3\82\93\E3\81\A7\E3\81\99\00"
@.str38 = private unnamed_addr constant [4 x i8] c"%s \00"
@.str39 = private unnamed_addr constant [2 x i8] c"\0A\00"

declare double @printf(i8*, ...)

declare double @__isoc99_scanf(i8*, ...)

declare double @sprintf(i8*, i8*, ...)

declare double @llvm.memset.p0i8.i64(i8* nocapture, i8, i64, i32, i1) nounwind

declare i32 @print(i8*)

declare i32 @input(i8*)

define double @f() {
entry:
  %i = alloca double
  %count__________count1 = alloca double
  %var_temp = load double* %i
  store double 1.000000e+00, double* %i
  %var_temp1 = load double* %count__________count1
  store double 1.000000e+00, double* %count__________count1
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %var_temp2 = load double* %count__________count1
  %cmp = fcmp ole double %var_temp2, 5.000000e+00
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %var_temp3 = load double* %i
  %0 = alloca [1 x i8]
  %print_str_temp = bitcast [1 x i8]* %0 to i8*
  %call_temp = call double @llvm.memset.p0i8.i64(i8* %print_str_temp, i8 0, i64 1, i32 1, i1 false)
  %rem_temp = frem double %var_temp3, 1.000000e+00
  %cmp4 = fcmp oeq double %rem_temp, 0.000000e+00
  br i1 %cmp4, label %int_arg, label %dec_arg

int_arg:                                          ; preds = %for.body
  %print_array = getelementptr inbounds [1 x i8]* %0, i32 0, i32 0
  %call_temp5 = call double (i8*, i8*, ...)* @sprintf(i8* %print_array, i8* getelementptr inbounds ([9 x i8]* @.str1, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8]* @.str2, i32 0, i32 0))
  %print_array6 = getelementptr inbounds [1 x i8]* %0, i32 0, i32 0
  %call_temp7 = call double (i8*, ...)* @printf(i8* %print_array6, double %var_temp3)
  br label %end_arg

dec_arg:                                          ; preds = %for.body
  %print_array8 = getelementptr inbounds [1 x i8]* %0, i32 0, i32 0
  %call_temp9 = call double (i8*, i8*, ...)* @sprintf(i8* %print_array8, i8* getelementptr inbounds ([7 x i8]* @.str3, i32 0, i32 0))
  %print_array10 = getelementptr inbounds [1 x i8]* %0, i32 0, i32 0
  %call_temp11 = call double (i8*, ...)* @printf(i8* %print_array10, double %var_temp3)
  br label %end_arg

end_arg:                                          ; preds = %dec_arg, %int_arg
  %var_temp12 = load double* %i
  %mul_tmp = fmul double %var_temp12, 1.000000e+00
  %1 = alloca [1 x i8]
  %print_str_temp13 = bitcast [1 x i8]* %1 to i8*
  %call_temp14 = call double @llvm.memset.p0i8.i64(i8* %print_str_temp13, i8 0, i64 1, i32 1, i1 false)
  %rem_temp15 = frem double %mul_tmp, 1.000000e+00
  %cmp16 = fcmp oeq double %rem_temp15, 0.000000e+00
  br i1 %cmp16, label %int_arg17, label %dec_arg18

int_arg17:                                        ; preds = %end_arg
  %print_array20 = getelementptr inbounds [1 x i8]* %1, i32 0, i32 0
  %call_temp21 = call double (i8*, i8*, ...)* @sprintf(i8* %print_array20, i8* getelementptr inbounds ([9 x i8]* @.str4, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8]* @.str5, i32 0, i32 0))
  %print_array22 = getelementptr inbounds [1 x i8]* %1, i32 0, i32 0
  %call_temp23 = call double (i8*, ...)* @printf(i8* %print_array22, double %mul_tmp)
  br label %end_arg19

dec_arg18:                                        ; preds = %end_arg
  %print_array24 = getelementptr inbounds [1 x i8]* %1, i32 0, i32 0
  %call_temp25 = call double (i8*, i8*, ...)* @sprintf(i8* %print_array24, i8* getelementptr inbounds ([7 x i8]* @.str6, i32 0, i32 0))
  %print_array26 = getelementptr inbounds [1 x i8]* %1, i32 0, i32 0
  %call_temp27 = call double (i8*, ...)* @printf(i8* %print_array26, double %mul_tmp)
  br label %end_arg19

end_arg19:                                        ; preds = %dec_arg18, %int_arg17
  %var_temp28 = load double* %i
  %add_tmp = fadd double %var_temp28, 5.000000e-01
  %2 = alloca [1 x i8]
  %print_str_temp29 = bitcast [1 x i8]* %2 to i8*
  %call_temp30 = call double @llvm.memset.p0i8.i64(i8* %print_str_temp29, i8 0, i64 1, i32 1, i1 false)
  %rem_temp31 = frem double %add_tmp, 1.000000e+00
  %cmp32 = fcmp oeq double %rem_temp31, 0.000000e+00
  br i1 %cmp32, label %int_arg33, label %dec_arg34

int_arg33:                                        ; preds = %end_arg19
  %print_array36 = getelementptr inbounds [1 x i8]* %2, i32 0, i32 0
  %call_temp37 = call double (i8*, i8*, ...)* @sprintf(i8* %print_array36, i8* getelementptr inbounds ([9 x i8]* @.str7, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8]* @.str8, i32 0, i32 0))
  %print_array38 = getelementptr inbounds [1 x i8]* %2, i32 0, i32 0
  %call_temp39 = call double (i8*, ...)* @printf(i8* %print_array38, double %add_tmp)
  br label %end_arg35

dec_arg34:                                        ; preds = %end_arg19
  %print_array40 = getelementptr inbounds [1 x i8]* %2, i32 0, i32 0
  %call_temp41 = call double (i8*, i8*, ...)* @sprintf(i8* %print_array40, i8* getelementptr inbounds ([7 x i8]* @.str9, i32 0, i32 0))
  %print_array42 = getelementptr inbounds [1 x i8]* %2, i32 0, i32 0
  %call_temp43 = call double (i8*, ...)* @printf(i8* %print_array42, double %add_tmp)
  br label %end_arg35

end_arg35:                                        ; preds = %dec_arg34, %int_arg33
  %var_temp44 = load double* %i
  %3 = alloca [1 x i8]
  %print_str_temp45 = bitcast [1 x i8]* %3 to i8*
  %call_temp46 = call double @llvm.memset.p0i8.i64(i8* %print_str_temp45, i8 0, i64 1, i32 1, i1 false)
  %print_array47 = getelementptr inbounds [1 x i8]* %3, i32 0, i32 0
  %call_temp48 = call double (i8*, i8*, ...)* @sprintf(i8* %print_array47, i8* getelementptr inbounds ([10 x i8]* @.str10, i32 0, i32 0), i8* getelementptr inbounds ([3 x i8]* @.str11, i32 0, i32 0), i8* getelementptr inbounds ([2 x i8]* @.str12, i32 0, i32 0))
  %print_array49 = getelementptr inbounds [1 x i8]* %3, i32 0, i32 0
  %call_temp50 = call double (i8*, ...)* @printf(i8* %print_array49, double %var_temp44)
  %4 = alloca [1 x i8]
  %print_str_temp51 = bitcast [1 x i8]* %4 to i8*
  %call_temp52 = call double @llvm.memset.p0i8.i64(i8* %print_str_temp51, i8 0, i64 1, i32 1, i1 false)
  %call_temp53 = call double (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @.str14, i32 0, i32 0), i8* getelementptr inbounds ([4 x i8]* @.str13, i32 0, i32 0))
  %5 = alloca [1 x i8]
  %print_str_temp54 = bitcast [1 x i8]* %5 to i8*
  %call_temp55 = call double @llvm.memset.p0i8.i64(i8* %print_str_temp54, i8 0, i64 1, i32 1, i1 false)
  %call_temp56 = call double (i8*, ...)* @printf(i8* getelementptr inbounds ([2 x i8]* @.str15, i32 0, i32 0))
  %var_temp57 = load double* %i
  %var_temp58 = load double* %i
  %add_tmp59 = fadd double %var_temp58, 1.000000e+00
  store double %add_tmp59, double* %i
  %var_temp60 = load double* %i
  %6 = alloca [1 x i8]
  %print_str_temp61 = bitcast [1 x i8]* %6 to i8*
  %call_temp62 = call double @llvm.memset.p0i8.i64(i8* %print_str_temp61, i8 0, i64 1, i32 1, i1 false)
  %rem_temp63 = frem double %var_temp60, 1.000000e+00
  %cmp64 = fcmp oeq double %rem_temp63, 0.000000e+00
  br i1 %cmp64, label %int_arg65, label %dec_arg66

int_arg65:                                        ; preds = %end_arg35
  %print_array68 = getelementptr inbounds [1 x i8]* %6, i32 0, i32 0
  %call_temp69 = call double (i8*, i8*, ...)* @sprintf(i8* %print_array68, i8* getelementptr inbounds ([9 x i8]* @.str17, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8]* @.str18, i32 0, i32 0))
  %print_array70 = getelementptr inbounds [1 x i8]* %6, i32 0, i32 0
  %call_temp71 = call double (i8*, ...)* @printf(i8* %print_array70, double %var_temp60)
  br label %end_arg67

dec_arg66:                                        ; preds = %end_arg35
  %print_array72 = getelementptr inbounds [1 x i8]* %6, i32 0, i32 0
  %call_temp73 = call double (i8*, i8*, ...)* @sprintf(i8* %print_array72, i8* getelementptr inbounds ([7 x i8]* @.str19, i32 0, i32 0))
  %print_array74 = getelementptr inbounds [1 x i8]* %6, i32 0, i32 0
  %call_temp75 = call double (i8*, ...)* @printf(i8* %print_array74, double %var_temp60)
  br label %end_arg67

end_arg67:                                        ; preds = %dec_arg66, %int_arg65
  %var_temp76 = load double* %i
  %mul_tmp77 = fmul double %var_temp76, 1.000000e+00
  %7 = alloca [1 x i8]
  %print_str_temp78 = bitcast [1 x i8]* %7 to i8*
  %call_temp79 = call double @llvm.memset.p0i8.i64(i8* %print_str_temp78, i8 0, i64 1, i32 1, i1 false)
  %rem_temp80 = frem double %mul_tmp77, 1.000000e+00
  %cmp81 = fcmp oeq double %rem_temp80, 0.000000e+00
  br i1 %cmp81, label %int_arg82, label %dec_arg83

int_arg82:                                        ; preds = %end_arg67
  %print_array85 = getelementptr inbounds [1 x i8]* %7, i32 0, i32 0
  %call_temp86 = call double (i8*, i8*, ...)* @sprintf(i8* %print_array85, i8* getelementptr inbounds ([9 x i8]* @.str20, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8]* @.str21, i32 0, i32 0))
  %print_array87 = getelementptr inbounds [1 x i8]* %7, i32 0, i32 0
  %call_temp88 = call double (i8*, ...)* @printf(i8* %print_array87, double %mul_tmp77)
  br label %end_arg84

dec_arg83:                                        ; preds = %end_arg67
  %print_array89 = getelementptr inbounds [1 x i8]* %7, i32 0, i32 0
  %call_temp90 = call double (i8*, i8*, ...)* @sprintf(i8* %print_array89, i8* getelementptr inbounds ([7 x i8]* @.str22, i32 0, i32 0))
  %print_array91 = getelementptr inbounds [1 x i8]* %7, i32 0, i32 0
  %call_temp92 = call double (i8*, ...)* @printf(i8* %print_array91, double %mul_tmp77)
  br label %end_arg84

end_arg84:                                        ; preds = %dec_arg83, %int_arg82
  %var_temp93 = load double* %i
  %add_tmp94 = fadd double %var_temp93, 5.000000e-01
  %8 = alloca [1 x i8]
  %print_str_temp95 = bitcast [1 x i8]* %8 to i8*
  %call_temp96 = call double @llvm.memset.p0i8.i64(i8* %print_str_temp95, i8 0, i64 1, i32 1, i1 false)
  %rem_temp97 = frem double %add_tmp94, 1.000000e+00
  %cmp98 = fcmp oeq double %rem_temp97, 0.000000e+00
  br i1 %cmp98, label %int_arg99, label %dec_arg100

int_arg99:                                        ; preds = %end_arg84
  %print_array102 = getelementptr inbounds [1 x i8]* %8, i32 0, i32 0
  %call_temp103 = call double (i8*, i8*, ...)* @sprintf(i8* %print_array102, i8* getelementptr inbounds ([9 x i8]* @.str23, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8]* @.str24, i32 0, i32 0))
  %print_array104 = getelementptr inbounds [1 x i8]* %8, i32 0, i32 0
  %call_temp105 = call double (i8*, ...)* @printf(i8* %print_array104, double %add_tmp94)
  br label %end_arg101

dec_arg100:                                       ; preds = %end_arg84
  %print_array106 = getelementptr inbounds [1 x i8]* %8, i32 0, i32 0
  %call_temp107 = call double (i8*, i8*, ...)* @sprintf(i8* %print_array106, i8* getelementptr inbounds ([7 x i8]* @.str25, i32 0, i32 0))
  %print_array108 = getelementptr inbounds [1 x i8]* %8, i32 0, i32 0
  %call_temp109 = call double (i8*, ...)* @printf(i8* %print_array108, double %add_tmp94)
  br label %end_arg101

end_arg101:                                       ; preds = %dec_arg100, %int_arg99
  %var_temp110 = load double* %i
  %9 = alloca [1 x i8]
  %print_str_temp111 = bitcast [1 x i8]* %9 to i8*
  %call_temp112 = call double @llvm.memset.p0i8.i64(i8* %print_str_temp111, i8 0, i64 1, i32 1, i1 false)
  %print_array113 = getelementptr inbounds [1 x i8]* %9, i32 0, i32 0
  %call_temp114 = call double (i8*, i8*, ...)* @sprintf(i8* %print_array113, i8* getelementptr inbounds ([10 x i8]* @.str26, i32 0, i32 0), i8* getelementptr inbounds ([2 x i8]* @.str27, i32 0, i32 0), i8* getelementptr inbounds ([2 x i8]* @.str28, i32 0, i32 0))
  %print_array115 = getelementptr inbounds [1 x i8]* %9, i32 0, i32 0
  %call_temp116 = call double (i8*, ...)* @printf(i8* %print_array115, double %var_temp110)
  %10 = alloca [1 x i8]
  %print_str_temp117 = bitcast [1 x i8]* %10 to i8*
  %call_temp118 = call double @llvm.memset.p0i8.i64(i8* %print_str_temp117, i8 0, i64 1, i32 1, i1 false)
  %call_temp119 = call double (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @.str30, i32 0, i32 0), i8* getelementptr inbounds ([4 x i8]* @.str29, i32 0, i32 0))
  %11 = alloca [1 x i8]
  %print_str_temp120 = bitcast [1 x i8]* %11 to i8*
  %call_temp121 = call double @llvm.memset.p0i8.i64(i8* %print_str_temp120, i8 0, i64 1, i32 1, i1 false)
  %call_temp122 = call double (i8*, ...)* @printf(i8* getelementptr inbounds ([2 x i8]* @.str31, i32 0, i32 0))
  %var_temp123 = load double* %i
  %cmp124 = fcmp oeq double %var_temp123, 5.000000e+00
  br i1 %cmp124, label %if.then, label %if.else

if.then:                                          ; preds = %end_arg101
  %12 = alloca [1 x i8]
  %print_str_temp125 = bitcast [1 x i8]* %12 to i8*
  %call_temp126 = call double @llvm.memset.p0i8.i64(i8* %print_str_temp125, i8 0, i64 1, i32 1, i1 false)
  %call_temp127 = call double (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @.str34, i32 0, i32 0), i8* getelementptr inbounds ([18 x i8]* @.str33, i32 0, i32 0))
  %13 = alloca [1 x i8]
  %print_str_temp128 = bitcast [1 x i8]* %13 to i8*
  %call_temp129 = call double @llvm.memset.p0i8.i64(i8* %print_str_temp128, i8 0, i64 1, i32 1, i1 false)
  %call_temp130 = call double (i8*, ...)* @printf(i8* getelementptr inbounds ([2 x i8]* @.str35, i32 0, i32 0))
  br label %if.end

if.else:                                          ; preds = %end_arg101
  %14 = alloca [1 x i8]
  %print_str_temp131 = bitcast [1 x i8]* %14 to i8*
  %call_temp132 = call double @llvm.memset.p0i8.i64(i8* %print_str_temp131, i8 0, i64 1, i32 1, i1 false)
  %call_temp133 = call double (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @.str38, i32 0, i32 0), i8* getelementptr inbounds ([27 x i8]* @.str37, i32 0, i32 0))
  %15 = alloca [1 x i8]
  %print_str_temp134 = bitcast [1 x i8]* %15 to i8*
  %call_temp135 = call double @llvm.memset.p0i8.i64(i8* %print_str_temp134, i8 0, i64 1, i32 1, i1 false)
  %call_temp136 = call double (i8*, ...)* @printf(i8* getelementptr inbounds ([2 x i8]* @.str39, i32 0, i32 0))
  br label %if.end

if.end:                                           ; preds = %if.then, %if.else
  br label %for.inc

for.inc:                                          ; preds = %if.end
  %var_temp137 = load double* %count__________count1
  %add_tmp138 = fadd double %var_temp137, 1.000000e+00
  store double %add_tmp138, double* %count__________count1
  br label %for.cond

for.end:                                          ; preds = %for.cond
  ret double 0.000000e+00
}

define i32 @main() {
entry:
  %call_temp = call double @f()
  ret i32 0
}
