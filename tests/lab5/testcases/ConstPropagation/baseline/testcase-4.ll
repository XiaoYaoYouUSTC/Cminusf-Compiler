; ModuleID = 'cminus'
source_filename = "/home/haiqwa/2020fall-compiler_cminus/tests/lab5/./testcases/ConstPropagation/testcase-4.cminus"

@a = global i32 zeroinitializer
declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define void @main() {
label_entry:
  store i32 3, i32* @a
  br label %label4
label4:                                                ; preds = %label_entry, %label35
  %op38 = phi i32 [ 0, %label_entry ], [ %op37, %label35 ]
  %op6 = icmp slt i32 %op38, 100000000
  %op7 = zext i1 %op6 to i32
  %op8 = icmp ne i32 %op7, 0
  br i1 %op8, label %label9, label %label28
label9:                                                ; preds = %label4
  %op10 = load i32, i32* @a
  %op18 = sitofp i32 %op10 to float
  %op19 = fadd float %op18, 0x4026bc77a0000000
  %op26 = fsub float %op19, 0x408c9dd680000000
  %op27 = fcmp une float %op26,0x0
  br i1 %op27, label %label32, label %label35
label28:                                                ; preds = %label4
  %op29 = load i32, i32* @a
  %op30 = mul i32 %op29, 2
  %op31 = add i32 %op30, 5
  call void @output(i32 %op31)
  ret void
label32:                                                ; preds = %label9
  %op33 = load i32, i32* @a
  %op34 = add i32 %op33, 1
  store i32 %op34, i32* @a
  br label %label35
label35:                                                ; preds = %label9, %label32
  %op37 = add i32 %op38, 1
  br label %label4
}
