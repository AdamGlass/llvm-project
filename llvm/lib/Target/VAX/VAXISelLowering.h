//===-- VAXISelLowering.h - VAX DAG Lowering Interface ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that VAX uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VAX_VAXISELLOWERING_H
#define LLVM_LIB_TARGET_VAX_VAXISELLOWERING_H

#include "VAX.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

  // Forward delcarations
  class VAXSubtarget;

  namespace VAXISD {
    enum NodeType : unsigned {
      // Start the numbering where the builtin ops and target ops leave off.
      FIRST_NUMBER = ISD::BUILTIN_OP_END,

    };
  }

  //===--------------------------------------------------------------------===//
  // TargetLowering Implementation
  //===--------------------------------------------------------------------===//
  class VAXTargetLowering : public TargetLowering
  {
  public:
    explicit VAXTargetLowering(const TargetMachine &TM,
                                 const VAXSubtarget &Subtarget);

  };
}

#endif
