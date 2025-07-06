; RUN: llc --mtriple=vax-unknown-netbsd < %s | FileCheck %s
define internal void @empty_void() {
entry:
  ret void
}

; CHECK-LABEL: empty_void:
; CHECK: PROCENTRYMASK
; CHECK: Ret

define internal i32 @empty_int() {
entry:
  ret i32 0
}
