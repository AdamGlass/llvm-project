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

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;
