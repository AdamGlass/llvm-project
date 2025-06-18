; RUN: opt < %s -passes=inline -inline-threshold=100 -S | FileCheck %s
; RUN: opt < %s -mtriple=aarch64-windows-msvc -passes=inline -inline-threshold=100 -S | FileCheck %s

define i32 @nosection_callee(i32 %x) {
  %x1 = add i32 %x, 1
  %x2 = add i32 %x1, 1
  %x3 = add i32 %x2, 1
  call void @extern()
  ret i32 %x3
}

define i32 @section_callee(i32 %x) section "FOO" {
  %x1 = add i32 %x, 1
  %x2 = add i32 %x1, 1
  %x3 = add i32 %x2, 1
  call void @extern()
  ret i32 %x3
}

define i32 @sectionpostfix_callee(i32 %x) section "FOO$BBBB" {
  %x1 = add i32 %x, 1
  %x2 = add i32 %x1, 1
  %x3 = add i32 %x2, 1
  call void @extern()
  ret i32 %x3
}

define i32 @paged_callee(i32 %x) section "PAGE" {
  %x1 = add i32 %x, 1
  %x2 = add i32 %x1, 1
  %x3 = add i32 %x2, 1
  call void @extern()
  ret i32 %x3
}

define i32 @pagedpostfix_callee(i32 %x) section "PAGE$aaa" {
  %x1 = add i32 %x, 1
  %x2 = add i32 %x1, 1
  %x3 = add i32 %x2, 1
  call void @extern()
  ret i32 %x3
}

define i32 @nosection_caller(i32 %y1) {
  %y2 = call i32 @nosection_callee(i32 %y1)
  %y3 = call i32 @section_callee(i32 %y2)
  %y4 = call i32 @sectionpostfix_callee(i32 %y3)
  %y5 = call i32 @paged_callee(i32 %y4)
  %y6 = call i32 @pagedpostfix_callee(i32 %y5)
  ret i32 %y6
}
  
define i32 @section_caller(i32 %y1) section "FOO" {
  %y2 = call i32 @nosection_callee(i32 %y1)
  %y3 = call i32 @section_callee(i32 %y2)
  %y4 = call i32 @sectionpostfix_callee(i32 %y3)
  %y5 = call i32 @paged_callee(i32 %y4)
  %y6 = call i32 @pagedpostfix_callee(i32 %y5)
  ret i32 %y6
}

define i32 @sectionpostfix_caller(i32 %y1) section "FOO$ZZZ" {
  %y2 = call i32 @nosection_callee(i32 %y1)
  %y3 = call i32 @section_callee(i32 %y2)
  %y4 = call i32 @sectionpostfix_callee(i32 %y3)
  %y5 = call i32 @paged_callee(i32 %y4)
  %y6 = call i32 @pagedpostfix_callee(i32 %y5)
  ret i32 %y6
}

define i32 @paged_caller(i32 %y1) section "PAGE" {
  %y2 = call i32 @nosection_callee(i32 %y1)
  %y3 = call i32 @section_callee(i32 %y2)
  %y4 = call i32 @sectionpostfix_callee(i32 %y3)
  %y5 = call i32 @paged_callee(i32 %y4)
  %y6 = call i32 @pagedpostfix_callee(i32 %y5)
  ret i32 %y6
}

define i32 @pagedpostfix_caller(i32 %y1) section "PAGE$ZZZ" {
  %y2 = call i32 @nosection_callee(i32 %y1)
  %y3 = call i32 @section_callee(i32 %y2)
  %y4 = call i32 @sectionpostfix_callee(i32 %y3)
  %y5 = call i32 @paged_callee(i32 %y4)
  %y6 = call i32 @pagedpostfix_callee(i32 %y5)
  ret i32 %y6
}


declare void @extern()

; CHECK-LABEL: @paged_callee