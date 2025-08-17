; RUN: llc -global-isel --mtriple=vax-unknown-netbsd < %s | FileCheck %s
define dso_local void @tfunc() {
  call void @vfunc()

; CHECK: movl $1, [[DATA1_R:r[0-9]+]]
; CHECK: movl $2, [[DATA2_R:r[0-9]+]]

; CHECK: movax vfunc, [[VFUNC_R:r[0-9]+]]
; CHECK-NEXT: calls $0, [[VFUNC_R]]

  call void @vfunc1(i32 noundef 1)

; CHECK: movax vfunc1, [[VFUNC1_R:r[0-9]+]]
; CHECK: calls $1, [[VFUNC1_R]]

  call void @vfunc2(i32 noundef 1, i32 noundef 2)

; CHECK: movax vfunc2, [[VFUNC2_R:r[0-9]+]]
; CHECK: pushl [[DATA2_R]]
; CHECK: pushl [[DATA1_R]]
; CHECK: calls $2, [[VFUNC2_R]]

  call void @vfunc7(i32 noundef 1, i32 noundef 2, i32 noundef 3, i32 noundef 4, i32 noundef 5, i32 noundef 6, i32 noundef 7)
; CHECK: calls $7, {{r[0-9]+}}
  call void @vfunc_zext_b(i8 noundef 1)
; CHECK: calls $1, {{r[0-9]+}}
  call void @vfunc_zext_w(i16 noundef 2)
; CHECK: calls $1, {{r[0-9]+}}
  call void @vfunc_zext_f32(float noundef 2.0)
; CHECK: calls $1, {{r[0-9]+}}

; NOTYET
;  call void @vfunc_zext_q(i64 noundef 8)
;  call void @vfunc_zext_i128(i128 noundef 16)
  ret void
}

declare void @vfunc(...)

declare void @vfunc1(i32 noundef)

declare void @vfunc2(i32 noundef, i32 noundef)

declare void @vfunc7(i32 noundef, i32 noundef, i32 noundef, i32 noundef, i32 noundef, i32 noundef, i32 noundef)

declare void @vfunc_zext_b(i8 noundef)

declare void @vfunc_zext_w(i16 noundef)

declare void @vfunc_zext_f32(float noundef)

declare void @vfunc_zext_q(i64 noundef)

declare void @vfunc_zext_i128(i128 noundef)
