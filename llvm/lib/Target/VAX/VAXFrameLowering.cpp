//===-- VAXFrameLowering.cpp - Frame info for VAX Target --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains VAX frame information that doesn't fit anywhere else
// cleanly...
//
//===----------------------------------------------------------------------===//

#include "VAXFrameLowering.h"
#include "VAX.h"
#include "VAXInstrInfo.h"
//#include "VAXMachineFunctionInfo.h"
#include "VAXSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetOptions.h"
#include <algorithm>

using namespace llvm;

static const unsigned FramePtr = VAX::FP;

//===----------------------------------------------------------------------===//
// VAXFrameLowering:
//===----------------------------------------------------------------------===//

VAXFrameLowering::VAXFrameLowering(const VAXSubtarget &sti)
    : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, Align(4), 0) {
  // Do nothing
}

bool VAXFrameLowering::hasFPImpl(const MachineFunction &MF) const {
  return MF.getTarget().Options.DisableFramePointerElim(MF) ||
         MF.getFrameInfo().hasVarSizedObjects();
}

void VAXFrameLowering::emitPrologue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  report_fatal_error("need to write prolog code");

}

void VAXFrameLowering::emitEpilogue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const {
  report_fatal_error("need to write epilogue code");
}
