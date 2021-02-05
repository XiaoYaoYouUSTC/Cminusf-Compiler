; ModuleID = 'fun.c'
source_filename = "fun.c"
;target information,copy from gcd_array.ll
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

;define callee
define dso_local i32 @callee(i32 %0) #0 {
	;define return variation
	%2=alloca i32
	;read and store a
	%3=alloca i32
	;set return variation
	store i32 0,i32* %2
	;set a
	store i32 %0,i32* %3
	;count 2*a
	%4=load i32,i32* %3
	%5=mul i32 2,%4
	;return 2*a
	ret i32 %5
}


;define main
define dso_local i32 @main() #0 {
	;define return variation
	%1=alloca i32
	store i32 0,i32* %1
	;call function callee,and get the result of callee(110)
	%2=call i32 @callee(i32 110)
	;return the number
	ret i32 %2
}

;other information,copy from gcd_array.ll
attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.1 "}
