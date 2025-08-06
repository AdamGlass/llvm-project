void vfunc();

void vfunc1(int a);

void vfunc2(int a, int b);

void vfunc7(int a, int b, int c, int d, int e, int f, int g);

void vfunc_zext_b(char a);
void vfunc_zext_w(unsigned short a);
void vfunc_zext_q(unsigned long a);
void vfunc_zext_i128(__int128 a);

void tfunc() {
    vfunc();
    vfunc1(1);
    vfunc2(1, 2);
    vfunc7(1, 2, 3, 4, 5, 6, 7);
    vfunc_zext_b(1);
    vfunc_zext_w(2);
    vfunc_zext_q(8);
    vfunc_zext_i128(16);
}
