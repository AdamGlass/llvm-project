; RUN: llc -global-isel --mtriple=vax-unknown-netbsd < %s | FileCheck %s
define dso_local i32 @addthis(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %4 = load i32, ptr %2, align 4
  %5 = add nsw i32 %3, %4
  ret i32 %5
}

; CHECK-LABEL: addthis

define dso_local i32 @addthisvalue(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %4 = add nsw i32 %3, 5
  ret i32 %4
}
