; RUN: llc --mtriple=vax-unknown-netbsd < %s | FileCheck %s
define internal void @empty_void() {
entry:
  ret void
}

; CHECK-LABEL: empty_void:
; CHECK: PROCENTRYMASK
; CHECK: Ret
