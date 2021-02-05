; ModuleID = 'cminus'
source_filename = "/home/haiqwa/2020fall-compiler_cminus/tests/lab5/./testcases/ConstPropagation/testcase-2.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define void @main() {
label_entry:
  br label %label6
label6:                                                ; preds = %label_entry, %label11
  %op65 = phi i32 [ 711082625, %label11 ], [ undef, %label_entry ]
  %op66 = phi i32 [ -599454271, %label11 ], [ undef, %label_entry ]
  %op67 = phi i32 [ 632274337, %label11 ], [ undef, %label_entry ]
  %op68 = phi i32 [ -1115555215, %label11 ], [ undef, %label_entry ]
  %op69 = phi i32 [ 0, %label_entry ], [ %op62, %label11 ]
  %op70 = phi i32 [ 0, %label_entry ], [ 457, %label11 ]
  %op8 = icmp slt i32 %op69, 100000000
  %op9 = zext i1 %op8 to i32
  %op10 = icmp ne i32 %op9, 0
  br i1 %op10, label %label11, label %label63
label11:                                                ; preds = %label6
  %op62 = add i32 %op69, 1
  br label %label6
label63:                                                ; preds = %label6
  call void @output(i32 %op65)
  ret void
}
