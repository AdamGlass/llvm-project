; RUN: llc -global-isel --mtriple=vax-unknown-netbsd < %s | FileCheck %s
define dso_local i32 @lfunc() #0 {
  ret i32 5
}

; CHECK-LABEL: lfunc

define dso_local i8 @bfunc() #0 {
  ret i8 5
}

; CHECK-LABEL: bfunc

define dso_local i16 @wfunc() #0 {
  ret i16 5
}

; CHECK-LABEL: wfunc

define dso_local float @ffunc() #0 {
  ret float 3.0
}

; CHECK-LABEL: ffunc

; NOTYET
; define dso_local i64 @qfunc() #0 {
; define dso_local double @qfunc() #0 {
