; RUN: llc -global-isel --mtriple=vax-unknown-netbsd < %s | FileCheck %s
define internal void @empty_void() {
entry:
  ret void
}

; CHECK-LABEL: empty_void:

define internal i32 @empty_int() {
entry:
  ret i32 0
}

; CHECK-LABEL: empty_int

define i32 @identity_arg(i32 %x) {
entry:
  ret i32 %x
}

; CHECK-LABEL: identity_arg

define i32 @square(i32 %x) {
entry:
  %result = mul i32 %x, %x
  ret i32 %result
}

; CHECK-LABEL: square
