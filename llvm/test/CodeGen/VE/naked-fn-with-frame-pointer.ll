; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py UTC_ARGS: --version 5
; RUN: llc < %s -mtriple ve | FileCheck %s -check-prefixes=CHECK

declare dso_local void @main()

define dso_local void @naked() naked "frame-pointer"="all" {
; CHECK-LABEL: naked:
; CHECK:       # %bb.0:
; CHECK-NEXT:    lea %s0, main@lo
; CHECK-NEXT:    and %s0, %s0, (32)0
; CHECK-NEXT:    lea.sl %s12, main@hi(, %s0)
; CHECK-NEXT:    bsic %s10, (, %s12)
  call void @main()
  unreachable
}

define dso_local void @normal() "frame-pointer"="all" {
; CHECK-LABEL: normal:
; CHECK:       # %bb.0:
; CHECK-NEXT:    st %s9, (, %s11)
; CHECK-NEXT:    st %s10, 8(, %s11)
; CHECK-NEXT:    or %s9, 0, %s11
; CHECK-NEXT:    lea %s11, -240(, %s11)
; CHECK-NEXT:    brge.l.t %s11, %s8, .LBB1_2
; CHECK-NEXT:  # %bb.1:
; CHECK-NEXT:    ld %s61, 24(, %s14)
; CHECK-NEXT:    or %s62, 0, %s0
; CHECK-NEXT:    lea %s63, 315
; CHECK-NEXT:    shm.l %s63, (%s61)
; CHECK-NEXT:    shm.l %s8, 8(%s61)
; CHECK-NEXT:    shm.l %s11, 16(%s61)
; CHECK-NEXT:    monc
; CHECK-NEXT:    or %s0, 0, %s62
; CHECK-NEXT:  .LBB1_2:
; CHECK-NEXT:    lea %s0, main@lo
; CHECK-NEXT:    and %s0, %s0, (32)0
; CHECK-NEXT:    lea.sl %s12, main@hi(, %s0)
; CHECK-NEXT:    bsic %s10, (, %s12)
  call void @main()
  unreachable
}
