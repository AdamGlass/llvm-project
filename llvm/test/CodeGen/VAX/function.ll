; RUN: llc -mtriple=VAX-netbsd < %s | FileCheck %s
define internal void @foo() {
entry:
  ret void
}

; CHECK-LAEBEL: foo
