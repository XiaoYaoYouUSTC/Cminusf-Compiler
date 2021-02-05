declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define void @main() {
label_entry:
  %op71 = mul i32 2, 2
  %op73 = mul i32 %op71, 2
  %op75 = mul i32 %op73, 2
  %op77 = mul i32 %op75, 2
  %op79 = mul i32 %op77, 2
  %op81 = mul i32 %op79, 2
  %op83 = mul i32 %op81, 2
  %op85 = mul i32 %op83, 2
  %op87 = mul i32 %op85, 2
  %op89 = sdiv i32 %op87, 2
  %op91 = sdiv i32 %op89, 2
  %op93 = sdiv i32 %op91, 2
  %op95 = sdiv i32 %op93, 2
  %op97 = sdiv i32 %op95, 2
  %op99 = sdiv i32 %op97, 2
  %op101 = sdiv i32 %op99, 2
  %op103 = sdiv i32 %op101, 2
  %op105 = sdiv i32 %op103, 2
  %op107 = sdiv i32 %op105, 2
  %op59 = icmp sgt i32 2, 1
  %op60 = zext i1 %op59 to i32
  %op61 = icmp ne i32 %op60, 0
  br label %label8
label8:                                                ; preds = %label_entry, %label22
  %op110 = phi i32 [ %op117, %label22 ], [ undef, %label_entry ]
  %op111 = phi i32 [ %op118, %label22 ], [ undef, %label_entry ]
  %op112 = phi i32 [ %op119, %label22 ], [ undef, %label_entry ]
  %op113 = phi i32 [ %op120, %label22 ], [ undef, %label_entry ]
  %op114 = phi i32 [ %op121, %label22 ], [ undef, %label_entry ]
  %op115 = phi i32 [ 0, %label_entry ], [ %op24, %label22 ]
  %op116 = phi i32 [ %op122, %label22 ], [ undef, %label_entry ]
  %op10 = icmp slt i32 %op115, 1000000
  %op11 = zext i1 %op10 to i32
  %op12 = icmp ne i32 %op11, 0
  br i1 %op12, label %label13, label %label14
label13:                                                ; preds = %label8
  br label %label16
label14:                                                ; preds = %label8
  call void @output(i32 %op110)
  ret void
label16:                                                ; preds = %label13, %label31
  %op117 = phi i32 [ %op110, %label13 ], [ %op123, %label31 ]
  %op118 = phi i32 [ %op111, %label13 ], [ %op124, %label31 ]
  %op119 = phi i32 [ %op112, %label13 ], [ %op125, %label31 ]
  %op120 = phi i32 [ %op113, %label13 ], [ %op126, %label31 ]
  %op121 = phi i32 [ %op114, %label13 ], [ %op127, %label31 ]
  %op122 = phi i32 [ 0, %label13 ], [ %op33, %label31 ]
  %op18 = icmp slt i32 %op122, 2
  %op19 = zext i1 %op18 to i32
  %op20 = icmp ne i32 %op19, 0
  br i1 %op20, label %label21, label %label22
label21:                                                ; preds = %label16
  br label %label25
label22:                                                ; preds = %label16
  %op24 = add i32 %op115, 1
  br label %label8
label25:                                                ; preds = %label21, %label40
  %op123 = phi i32 [ %op117, %label21 ], [ %op129, %label40 ]
  %op124 = phi i32 [ %op118, %label21 ], [ %op130, %label40 ]
  %op125 = phi i32 [ %op119, %label21 ], [ %op131, %label40 ]
  %op126 = phi i32 [ %op120, %label21 ], [ %op132, %label40 ]
  %op127 = phi i32 [ 0, %label21 ], [ %op42, %label40 ]
  %op128 = phi i32 [ %op122, %label21 ], [ %op133, %label40 ]
  %op27 = icmp slt i32 %op127, 2
  %op28 = zext i1 %op27 to i32
  %op29 = icmp ne i32 %op28, 0
  br i1 %op29, label %label30, label %label31
label30:                                                ; preds = %label25
  br label %label34
label31:                                                ; preds = %label25
  %op33 = add i32 %op128, 1
  br label %label16
label34:                                                ; preds = %label30, %label49
  %op129 = phi i32 [ %op123, %label30 ], [ %op134, %label49 ]
  %op130 = phi i32 [ %op124, %label30 ], [ %op135, %label49 ]
  %op131 = phi i32 [ %op125, %label30 ], [ %op136, %label49 ]
  %op132 = phi i32 [ 0, %label30 ], [ %op51, %label49 ]
  %op133 = phi i32 [ %op128, %label30 ], [ %op137, %label49 ]
  %op36 = icmp slt i32 %op132, 2
  %op37 = zext i1 %op36 to i32
  %op38 = icmp ne i32 %op37, 0
  br i1 %op38, label %label39, label %label40
label39:                                                ; preds = %label34
  br label %label43
label40:                                                ; preds = %label34
  %op42 = add i32 %op127, 1
  br label %label25
label43:                                                ; preds = %label39, %label62
  %op134 = phi i32 [ %op129, %label39 ], [ %op138, %label62 ]
  %op135 = phi i32 [ %op130, %label39 ], [ %op139, %label62 ]
  %op136 = phi i32 [ 0, %label39 ], [ %op64, %label62 ]
  %op137 = phi i32 [ %op133, %label39 ], [ %op140, %label62 ]
  %op45 = icmp slt i32 %op136, 2
  %op46 = zext i1 %op45 to i32
  %op47 = icmp ne i32 %op46, 0
  br i1 %op47, label %label48, label %label49
label48:                                                ; preds = %label43
  br label %label52
label49:                                                ; preds = %label43
  %op51 = add i32 %op132, 1
  br label %label34
label52:                                                ; preds = %label48, %label68
  %op138 = phi i32 [ %op134, %label48 ], [ %op107, %label68 ]
  %op139 = phi i32 [ 0, %label48 ], [ %op109, %label68 ]
  %op140 = phi i32 [ %op137, %label48 ], [ %op141, %label68 ]
  %op54 = icmp slt i32 %op139, 2
  %op55 = zext i1 %op54 to i32
  %op56 = icmp ne i32 %op55, 0
  br i1 %op56, label %label57, label %label62
label57:                                                ; preds = %label52
  br i1 %op61, label %label65, label %label68
label62:                                                ; preds = %label52
  %op64 = add i32 %op136, 1
  br label %label43
label65:                                                ; preds = %label57
  %op67 = add i32 %op140, 1
  br label %label68
label68:                                                ; preds = %label57, %label65
  %op141 = phi i32 [ %op140, %label57 ], [ %op67, %label65 ]
  %op109 = add i32 %op139, 1
  br label %label52
}
