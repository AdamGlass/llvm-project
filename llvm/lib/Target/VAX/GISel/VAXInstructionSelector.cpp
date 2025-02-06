//===- VAXInstructionSelector.cpp --------------------------------*- C++ -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This file implements the targeting of the InstructionSelector class for VAX.
//===----------------------------------------------------------------------===//

#include "VAXInstrInfo.h"
#include "VAXRegisterBankInfo.h"
#include "VAXSubtarget.h"
#include "VAXTargetMachine.h"
#include "llvm/CodeGen/GlobalISel/GIMatchTableExecutorImpl.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelector.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/IR/IntrinsicsVAX.h"

#define DEBUG_TYPE "vax-gisel"

using namespace llvm;

namespace {

#define GET_GLOBALISEL_PREDICATE_BITSET
#include "VAXGenGlobalISel.inc"
#undef GET_GLOBALISEL_PREDICATE_BITSET

class VAXInstructionSelector : public InstructionSelector {
public:
  VAXInstructionSelector(const VAXTargetMachine &TM, const VAXSubtarget &STI,
                         const VAXRegisterBankInfo &RBI);

  bool select(MachineInstr &I) override;
  static const char *getName() { return DEBUG_TYPE; }

private:
  /// tblgen generated 'select' implementation that is used as the initial
  /// selector for the patterns that do not require complex C++.
  bool selectImpl(MachineInstr &I, CodeGenCoverage &CoverageInfo) const;

  const VAXInstrInfo &TII;
  const VAXRegisterInfo &TRI;
  const VAXRegisterBankInfo &RBI;

#define GET_GLOBALISEL_PREDICATES_DECL
#include "VAXGenGlobalISel.inc"
#undef GET_GLOBALISEL_PREDICATES_DECL

#define GET_GLOBALISEL_TEMPORARIES_DECL
#include "VAXGenGlobalISel.inc"
#undef GET_GLOBALISEL_TEMPORARIES_DECL
};

} // namespace

#define GET_GLOBALISEL_IMPL
#include "VAXGenGlobalISel.inc"
#undef GET_GLOBALISEL_IMPL

VAXInstructionSelector::VAXInstructionSelector(const VAXTargetMachine &TM,
                                               const VAXSubtarget &STI,
                                               const VAXRegisterBankInfo &RBI)
    : TII(*STI.getInstrInfo()), TRI(*STI.getRegisterInfo()), RBI(RBI),
#define GET_GLOBALISEL_PREDICATES_INIT
#include "VAXGenGlobalISel.inc"
#undef GET_GLOBALISEL_PREDICATES_INIT
#define GET_GLOBALISEL_TEMPORARIES_INIT
#include "VAXGenGlobalISel.inc"
#undef GET_GLOBALISEL_TEMPORARIES_INIT
{
}

bool VAXInstructionSelector::select(MachineInstr &I) {
  if (!isPreISelGenericOpcode(I.getOpcode()))
    return true;
  if (selectImpl(I, *CoverageInfo))
    return true;
  return false;
}

namespace llvm {
InstructionSelector *
createVAXInstructionSelector(const VAXTargetMachine &TM,
                             const VAXSubtarget &Subtarget,
                             const VAXRegisterBankInfo &RBI) {
  return new VAXInstructionSelector(TM, Subtarget, RBI);
}
} // namespace llvm
