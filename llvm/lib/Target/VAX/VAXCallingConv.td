//===- VAXCallingConv.td - Calling Conventions for VAX -*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for VAX architecture.
//===----------------------------------------------------------------------===//

// Need to account for
// zoo() {}
//        .word 0
//        subl2 $4,%sp
//        ret
//

//===----------------------------------------------------------------------===//
// VAX Argument Calling Conventions
//===----------------------------------------------------------------------===//
def CC_VAX : CallingConv<[
  // Promote i8/i16 arguments to i32.
  CCIfType<[i8, i16], CCPromoteToType<i32>>,

  /// Handles byval parameters.  What sets R1?
  CCIfByVal<CCPassByVal<4, 4>>,

  // Otherwise they are assigned to the stack in 4-byte aligned units.
  CCIfType<[i32],  CCAssignToStack<4, 4>>
]>;

// VAX 32-bit C return-value convention.
def RetCC_VAX : CallingConv<[
  // Specify two registers to allow returning 64-bit results that have already
  // been lowered to 2 32-bit values.
  CCIfType<[i32], CCAssignToReg<[R0, R1]>>
]>;
