//===-- VAXTargetTransformInfo.h - VAX specific TTI ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This file a TargetTransformInfo::Concept conforming object specific to the
/// VAX target machine. It uses the target's detailed information to
/// provide more precise answers to certain TTI queries, while letting the
/// target independent and default TTI implementations handle the rest.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VAX_VAXTARGETTRANSFORMINFO_H
#define LLVM_LIB_TARGET_VAX_VAXTARGETTRANSFORMINFO_H

#include "VAX.h"
#include "VAXTargetMachine.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

class VAXTTIImpl : public BasicTTIImplBase<VAXTTIImpl> {
  typedef BasicTTIImplBase<VAXTTIImpl> BaseT;
  typedef TargetTransformInfo TTI;
  friend BaseT;

  const VAXSubtarget *ST;
  const VAXTargetLowering *TLI;

  const VAXSubtarget *getST() const { return ST; }
  const VAXTargetLowering *getTLI() const { return TLI; }

public:
  explicit VAXTTIImpl(const VAXTargetMachine *TM, const Function &F)
      : BaseT(TM, F.getDataLayout()), ST(TM->getSubtargetImpl()),
        TLI(ST->getTargetLowering()) {}

  unsigned getNumberOfRegisters(unsigned ClassID) const {
    bool Vector = (ClassID == 1);
    if (Vector) {
      return 0;
    }
    return 16;
  }
};

} // end namespace llvm

#endif
