// Define intrinsics written by hand

// VAX Intrinsics

let TargetPrefix = "vax" in {

def int_vax_hlt : Intrinsic<[], [llvm_i32_ty],
    [IntrNoMem, IntrHasSideEffects, IntrNoReturn, IntrCold, ImmArg<ArgIndex<0>>]>;

}
