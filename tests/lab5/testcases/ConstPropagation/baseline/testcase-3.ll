; ModuleID = 'cminus'
source_filename = "/home/haiqwa/2020fall-compiler_cminus/tests/lab5/./testcases/ConstPropagation/testcase-3.cminus"

@opa = global i32 zeroinitializer
@opb = global i32 zeroinitializer
@opc = global i32 zeroinitializer
@opd = global i32 zeroinitializer
declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @max() {
label_entry:
  store i32 0, i32* @opa
  store i32 40320, i32* @opb
  store i32 362880, i32* @opc
  store i32 1814400, i32* @opd
  br label %label33
label33:                                                ; preds = %label_entry
  %op34 = load i32, i32* @opb
  %op35 = load i32, i32* @opc
  %op36 = icmp slt i32 %op34, %op35
  %op37 = zext i1 %op36 to i32
  %op38 = icmp ne i32 %op37, 0
  br i1 %op38, label %label40, label %label46
label39:                                                ; preds = %label46
  ret i32 0
label40:                                                ; preds = %label33
  %op41 = load i32, i32* @opc
  %op42 = load i32, i32* @opd
  %op43 = icmp slt i32 %op41, %op42
  %op44 = zext i1 %op43 to i32
  %op45 = icmp ne i32 %op44, 0
  br i1 %op45, label %label47, label %label49
label46:                                                ; preds = %label33, %label49
  br label %label39
label47:                                                ; preds = %label40
  %op48 = load i32, i32* @opd
  ret i32 %op48
label49:                                                ; preds = %label40
  br label %label46
}
define void @main() {
label_entry:
  br label %label1
label1:                                                ; preds = %label_entry, %label6
  %op15 = phi i32 [ 0, %label_entry ], [ %op9, %label6 ]
  %op3 = icmp slt i32 %op15, 200000000
  %op4 = zext i1 %op3 to i32
  %op5 = icmp ne i32 %op4, 0
  br i1 %op5, label %label6, label %label10
label6:                                                ; preds = %label1
  %op7 = call i32 @max()
  %op9 = add i32 %op15, 1
  br label %label1
label10:                                                ; preds = %label1
  %op11 = load i32, i32* @opa
  call void @output(i32 %op11)
  %op12 = load i32, i32* @opb
  call void @output(i32 %op12)
  %op13 = load i32, i32* @opc
  call void @output(i32 %op13)
  %op14 = load i32, i32* @opd
  call void @output(i32 %op14)
  ret void
}
