; ModuleID = 'cminus'
source_filename = "/home/haiqwa/2020fall-compiler_cminus/tests/lab5/./testcases/LoopInvHoist/testcase-6.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define void @main() {
label_entry:
  %op20 = mul i32 2, 2
  %op22 = mul i32 %op20, 2
  %op24 = mul i32 %op22, 2
  %op26 = mul i32 %op24, 2
  %op28 = mul i32 %op26, 2
  %op30 = mul i32 %op28, 2
  %op32 = mul i32 %op30, 2
  %op34 = mul i32 %op32, 2
  %op36 = mul i32 %op34, 2
  %op38 = sdiv i32 %op36, 2
  %op40 = sdiv i32 %op38, 2
  %op42 = sdiv i32 %op40, 2
  %op44 = sdiv i32 %op42, 2
  %op46 = sdiv i32 %op44, 2
  %op48 = sdiv i32 %op46, 2
  %op50 = sdiv i32 %op48, 2
  %op52 = sdiv i32 %op50, 2
  %op54 = sdiv i32 %op52, 2
  %op56 = sdiv i32 %op54, 2
  br label %label4
label4:                                                ; preds = %label_entry, %label59
  %op62 = phi i32 [ %op65, %label59 ], [ undef, %label_entry ]
  %op63 = phi i32 [ 0, %label_entry ], [ %op61, %label59 ]
  %op64 = phi i32 [ %op66, %label59 ], [ undef, %label_entry ]
  %op6 = icmp slt i32 %op63, 10000000
  %op7 = zext i1 %op6 to i32
  %op8 = icmp ne i32 %op7, 0
  br i1 %op8, label %label9, label %label10
label9:                                                ; preds = %label4
  br label %label12
label10:                                                ; preds = %label4
  call void @output(i32 %op62)
  ret void
label12:                                                ; preds = %label9, %label17
  %op65 = phi i32 [ %op62, %label9 ], [ %op56, %label17 ]
  %op66 = phi i32 [ 0, %label9 ], [ %op58, %label17 ]
  %op14 = icmp slt i32 %op66, 2
  %op15 = zext i1 %op14 to i32
  %op16 = icmp ne i32 %op15, 0
  br i1 %op16, label %label17, label %label59
label17:                                                ; preds = %label12
  %op58 = add i32 %op66, 1
  br label %label12
label59:                                                ; preds = %label12
  %op61 = add i32 %op63, 1
  br label %label4
}
