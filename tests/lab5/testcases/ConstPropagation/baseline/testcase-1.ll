; ModuleID = 'cminus'
source_filename = "/home/haiqwa/2020fall-compiler_cminus/tests/lab5/./testcases/ConstPropagation/testcase-1.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define void @main() {
label_entry:
  br label %label2
label2:                                                ; preds = %label_entry, %label7
  %op78 = phi i32 [ 0, %label_entry ], [ %op73, %label7 ]
  %op79 = phi i32 [ 0, %label_entry ], [ 34, %label7 ]
  %op4 = icmp slt i32 %op78, 100000000
  %op5 = zext i1 %op4 to i32
  %op6 = icmp ne i32 %op5, 0
  br i1 %op6, label %label7, label %label74
label7:                                                ; preds = %label2
  %op73 = add i32 %op78, 1
  br label %label2
label74:                                                ; preds = %label2
  %op77 = mul i32 %op79, %op79
  call void @output(i32 %op77)
  ret void
}
