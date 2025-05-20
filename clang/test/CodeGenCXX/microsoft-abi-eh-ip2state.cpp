// RUN: %clang_cl -c --target=x86_64-windows-msvc -O2 /EHsc \
// RUN:   -Xclang=-import-call-optimization \
// RUN:   /clang:-S /clang:-o- %s 2>&1 \
// RUN:   | FileCheck %s

void might_throw();
void other_func();

extern "C" void __declspec(dllimport) some_dll_import();

class HasDtor {
    int x;

public:
    explicit HasDtor(int x);
    ~HasDtor();
};

void normal_has_regions() {
    {
        HasDtor hd{42};
        might_throw();
    }

    other_func();
}
// CHECK: ?normal_has_regions@@YAXXZ
// CHECK: call "??0HasDtor@@QEAA@H@Z"
// CHECK-NEXT: nop

void dll_has_regions() {
    {
        HasDtor hd{42};
        some_dll_import();
    }

    other_func();
}

// CHECK: ?dll_has_regions@@YAXXZ
// CHECK: .Limpcall
// CHECK-NEXT: rex64
// CHECK-NEXT: call __imp_some_dll_import
// CHECK-NEXT: nop dword ptr
// CHECK-NEXT: nop
