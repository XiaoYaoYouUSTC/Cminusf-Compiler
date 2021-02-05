; ModuleID = 'while.c'
source_filename = "while.c"
;target information,copy from gcd_array.ll
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

;define main function
define dso_local i32 @main() #0 {
	;define return variation
	%1=alloca i32
	;define a
	%2=alloca i32
	;define i
	%3=alloca i32
	;set return variation 0
	store i32 0,i32* %1
	;set a=10
	store i32 10,i32* %2
	;set i=0
	store i32 0,i32* %3
	;compare i with 10
	;decide whether to enter the loop
	%4=load i32,i32* %3
	%5=icmp slt i32 %4,10
	br i1 %5,label %6,label %14
	
;the second basic block
;the action of while
6:			;preds=%0,%6
	;set i=i+1
	%7=load i32,i32* %3
	%8=add i32 %7,1
	store i32 %8,i32* %3
	;set a=a+i
	%9=load i32,i32* %2
	%10=load i32,i32* %3
	%11=add i32 %9,%10
	store i32 %11,i32* %2
	;compare i with 10
	;decide whether to leave the loop
	%12=load i32,i32* %3
	%13=icmp slt i32 %12,10
	br i1 %13,label %6,label %14
	
;the third basic block
14:			;preds=%0,%6
	;return a
	%15=load i32,i32* %2
	ret i32 %15
	
}



;other information,copy from gcd_array.ll
attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.1 "}
