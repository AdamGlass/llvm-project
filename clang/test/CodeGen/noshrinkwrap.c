// RUN: %clang_cc1 -triple aarch64-none-linux-gnu -ffreestanding -emit-llvm -fdeclspec -w -o - %s | FileCheck %s

void bar();

void shrinkwrap(int x) {
    if (x > 0) {
        asm volatile("" ::: "x19"); // simulate use of callee-saved register
        bar();
    } else {
        bar();
    }
}

// CHECK-LABEL: define dso_local void @shrinkwrap(i32 noundef %x)

__declspec(noshrinkwrap)
void noshrinkwrap(int x) {
    if (x > 0) {
        asm volatile("" ::: "x19"); // simulate use of callee-saved register
        bar();
    } else {
        bar();
    }
}

// CHECK-LABEL: define dso_local void @noshrinkwrap(i32 noundef %x)
// CHECK: attributes #2 = { noinline noshrinkwrap
