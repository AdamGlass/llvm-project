; RUN: llc -global-isel --mtriple=vax-unknown-netbsd < %s | FileCheck %s
define dso_local void @tfunc() {
  call void @vfunc()
  call void @vfunc1(i32 noundef 1)
  call void @vfunc2(i32 noundef 1, i32 noundef 2)
  call void @vfunc7(i32 noundef 1, i32 noundef 2, i32 noundef 3, i32 noundef 4, i32 noundef 5, i32 noundef 6, i32 noundef 7)
  call void @vfunc_zext_b(i8 noundef 1)
  call void @vfunc_zext_w(i16 noundef 2)
  call void @vfunc_zext_f32(float noundef 2.0)
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
