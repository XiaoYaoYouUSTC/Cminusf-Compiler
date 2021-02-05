declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define void @main() {
label_entry:
  %op60 = mul i32 2, 2
  %op62 = mul i32 %op60, 2
  %op64 = mul i32 %op62, 2
  %op66 = mul i32 %op64, 2
  %op68 = mul i32 %op66, 2
  %op70 = mul i32 %op68, 2
  %op72 = mul i32 %op70, 2
  %op74 = mul i32 %op72, 2
  %op76 = mul i32 %op74, 2
  %op78 = sdiv i32 %op76, 2
  %op80 = sdiv i32 %op78, 2
  %op82 = sdiv i32 %op80, 2
  %op84 = sdiv i32 %op82, 2
  %op86 = sdiv i32 %op84, 2
  %op88 = sdiv i32 %op86, 2
  %op90 = sdiv i32 %op88, 2
  %op92 = sdiv i32 %op90, 2
  %op94 = sdiv i32 %op92, 2
  %op96 = sdiv i32 %op94, 2
  br label %label8
label8:                                                ; preds = %label_entry, %label22
  %op102 = phi i32 [ %op109, %label22 ], [ undef, %label_entry ]
  %op103 = phi i32 [ %op110, %label22 ], [ undef, %label_entry ]
  %op104 = phi i32 [ %op111, %label22 ], [ undef, %label_entry ]
  %op105 = phi i32 [ %op112, %label22 ], [ undef, %label_entry ]
  %op106 = phi i32 [ %op113, %label22 ], [ undef, %label_entry ]
  %op107 = phi i32 [ 0, %label_entry ], [ %op24, %label22 ]
  %op108 = phi i32 [ %op114, %label22 ], [ undef, %label_entry ]
  %op10 = icmp slt i32 %op107, 1000000
  %op11 = zext i1 %op10 to i32
  %op12 = icmp ne i32 %op11, 0
  br i1 %op12, label %label13, label %label14
label13:                                                ; preds = %label8
  br label %label16
label14:                                                ; preds = %label8
  call void @output(i32 %op102)
  ret void
label16:                                                ; preds = %label13, %label31
  %op109 = phi i32 [ %op102, %label13 ], [ %op115, %label31 ]
  %op110 = phi i32 [ %op103, %label13 ], [ %op116, %label31 ]
  %op111 = phi i32 [ %op104, %label13 ], [ %op117, %label31 ]
  %op112 = phi i32 [ %op105, %label13 ], [ %op118, %label31 ]
  %op113 = phi i32 [ %op106, %label13 ], [ %op119, %label31 ]
  %op114 = phi i32 [ 0, %label13 ], [ %op33, %label31 ]
  %op18 = icmp slt i32 %op114, 2
  %op19 = zext i1 %op18 to i32
  %op20 = icmp ne i32 %op19, 0
  br i1 %op20, label %label21, label %label22
label21:                                                ; preds = %label16
  br label %label25
label22:                                                ; preds = %label16
  %op24 = add i32 %op107, 1
  br label %label8
label25:                                                ; preds = %label21, %label40
  %op115 = phi i32 [ %op109, %label21 ], [ %op120, %label40 ]
  %op116 = phi i32 [ %op110, %label21 ], [ %op121, %label40 ]
  %op117 = phi i32 [ %op111, %label21 ], [ %op122, %label40 ]
  %op118 = phi i32 [ %op112, %label21 ], [ %op123, %label40 ]
  %op119 = phi i32 [ 0, %label21 ], [ %op42, %label40 ]
  %op27 = icmp slt i32 %op119, 2
  %op28 = zext i1 %op27 to i32
  %op29 = icmp ne i32 %op28, 0
  br i1 %op29, label %label30, label %label31
label30:                                                ; preds = %label25
  br label %label34
label31:                                                ; preds = %label25
  %op33 = add i32 %op114, 1
  br label %label16
label34:                                                ; preds = %label30, %label49
  %op120 = phi i32 [ %op115, %label30 ], [ %op124, %label49 ]
  %op121 = phi i32 [ %op116, %label30 ], [ %op125, %label49 ]
  %op122 = phi i32 [ %op117, %label30 ], [ %op126, %label49 ]
  %op123 = phi i32 [ 0, %label30 ], [ %op51, %label49 ]
  %op36 = icmp slt i32 %op123, 2
  %op37 = zext i1 %op36 to i32
  %op38 = icmp ne i32 %op37, 0
  br i1 %op38, label %label39, label %label40
label39:                                                ; preds = %label34
  br label %label43
label40:                                                ; preds = %label34
  %op42 = add i32 %op119, 1
  br label %label25
label43:                                                ; preds = %label39, %label99
  %op124 = phi i32 [ %op120, %label39 ], [ %op127, %label99 ]
  %op125 = phi i32 [ %op121, %label39 ], [ %op128, %label99 ]
  %op126 = phi i32 [ 0, %label39 ], [ %op101, %label99 ]
  %op45 = icmp slt i32 %op126, 2
  %op46 = zext i1 %op45 to i32
  %op47 = icmp ne i32 %op46, 0
  br i1 %op47, label %label48, label %label49
label48:                                                ; preds = %label43
  br label %label52
label49:                                                ; preds = %label43
  %op51 = add i32 %op123, 1
  br label %label34
label52:                                                ; preds = %label48, %label57
  %op127 = phi i32 [ %op124, %label48 ], [ %op96, %label57 ]
  %op128 = phi i32 [ 0, %label48 ], [ %op98, %label57 ]
  %op54 = icmp slt i32 %op128, 2
  %op55 = zext i1 %op54 to i32
  %op56 = icmp ne i32 %op55, 0
  br i1 %op56, label %label57, label %label99
label57:                                                ; preds = %label52
  %op98 = add i32 %op128, 1
  br label %label52
label99:                                                ; preds = %label52
  %op101 = add i32 %op126, 1
  br label %label43
}
