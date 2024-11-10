// Define intrinsics written by hand

// VEL Intrinsic instructions.
let TargetPrefix = "vax" in {
  def int_ve_vl_pack_f32p : ClangBuiltin<"__builtin_ve_vl_pack_f32p">,
                            DefaultAttrsIntrinsic<[llvm_i64_ty], [llvm_ptr_ty, llvm_ptr_ty],
}

// Define intrinsics automatically generated
include "llvm/IR/IntrinsicsVAX.gen.td"
