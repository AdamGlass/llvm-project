//===-- VAX.h - Top-level interface for VAX representation --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// VAX back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VAX_VAX_H
#define LLVM_LIB_TARGET_VAX_VAX_H

#include "MCTargetDesc/VAXMCTargetDesc.h"
#include "llvm/PassRegistry.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
#if 0
  class FunctionPass;
  class ModulePass;
  class PassRegistry;
  class TargetMachine;
  class VAXTargetMachine;

  void initializeVAXLowerThreadLocalPass(PassRegistry &p);

  FunctionPass *createVAXFrameToArgsOffsetEliminationPass();
  FunctionPass *createVAXISelDag(VAXTargetMachine &TM,
                                   CodeGenOptLevel OptLevel);
  ModulePass *createVAXLowerThreadLocalPass();
  void initializeVAXDAGToDAGISelLegacyPass(PassRegistry &);
#endif
} // end namespace llvm;

#endif
