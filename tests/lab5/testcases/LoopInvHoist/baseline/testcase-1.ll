; ModuleID = 'cminus'
source_filename = "/home/haiqwa/2020fall-compiler_cminus/tests/lab5/./testcases/LoopInvHoist/testcase-5.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define void @main() {
label_entry:
  br label %label3
label3:                                                ; preds = %label_entry, %label58
  %op61 = phi i32 [ %op64, %label58 ], [ undef, %label_entry ]
  %op62 = phi i32 [ 1, %label_entry ], [ %op60, %label58 ]
  %op63 = phi i32 [ %op65, %label58 ], [ undef, %label_entry ]
  %op5 = icmp slt i32 %op62, 10000
  %op6 = zext i1 %op5 to i32
  %op7 = icmp ne i32 %op6, 0
  br i1 %op7, label %label8, label %label9
label8:                                                ; preds = %label3
  %op19 = mul i32 %op62, %op62
  %op21 = mul i32 %op19, %op62
  %op23 = mul i32 %op21, %op62
  %op25 = mul i32 %op23, %op62
  %op27 = mul i32 %op25, %op62
  %op29 = mul i32 %op27, %op62
  %op31 = mul i32 %op29, %op62
  %op33 = mul i32 %op31, %op62
  %op35 = mul i32 %op33, %op62
  %op37 = sdiv i32 %op35, %op62
  %op39 = sdiv i32 %op37, %op62
  %op41 = sdiv i32 %op39, %op62
  %op43 = sdiv i32 %op41, %op62
  %op45 = sdiv i32 %op43, %op62
  %op47 = sdiv i32 %op45, %op62
  %op49 = sdiv i32 %op47, %op62
  %op51 = sdiv i32 %op49, %op62
  %op53 = sdiv i32 %op51, %op62
  %op55 = sdiv i32 %op53, %op62
  br label %label11
label9:                                                ; preds = %label3
  call void @output(i32 %op61)
  ret void
label11:                                                ; preds = %label8, %label16
  %op64 = phi i32 [ %op61, %label8 ], [ %op55, %label16 ]
  %op65 = phi i32 [ 0, %label8 ], [ %op57, %label16 ]
  %op13 = icmp slt i32 %op65, 10000
  %op14 = zext i1 %op13 to i32
  %op15 = icmp ne i32 %op14, 0
  br i1 %op15, label %label16, label %label58
label16:                                                ; preds = %label11
  %op57 = add i32 %op65, 1
  br label %label11
label58:                                                ; preds = %label11
  %op60 = add i32 %op62, 1
  br label %label3
}
