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
// #include "VAXMachineFunctionInfo.h"
#include "VAXSubtarget.h"
#include "llvm/CodeGen/LivePhysRegs.h"
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

#define DEBUG_TYPE "vaxframelowering"
static const unsigned FramePtr = VAX::FP;

//===----------------------------------------------------------------------===//
// VAXFrameLowering:
//===----------------------------------------------------------------------===//

VAXFrameLowering::VAXFrameLowering(const VAXSubtarget &sti)
    : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, Align(4), 0),
      STI(sti) {}

bool VAXFrameLowering::hasFPImpl(const MachineFunction &MF) const {
  return MF.getTarget().Options.DisableFramePointerElim(MF) ||
         MF.getFrameInfo().hasVarSizedObjects();
}

void VAXFrameLowering::emitPrologue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {

  assert(&MBB == &MF.front() && "Shrink-wrapping not yet implemented");
  MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineBasicBlock::iterator MBBI = MBB.begin();
  const VAXInstrInfo &TII = *STI.getInstrInfo();
  const VAXRegisterInfo &TRI = *STI.getRegisterInfo();
  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  MCRegister FP = TRI.getFrameRegister(MF);
  const MCRegisterInfo *MRI = MF.getContext().getRegisterInfo();
  uint64_t StackSize = MFI.getStackSize();
  LivePhysRegs LiveRegs(TRI);
  uint16_t EntryMask;

  for (const auto &CSI : MFI.getCalleeSavedInfo()) {
    unsigned Reg = CSI.getReg();
    EntryMask |= (1 << Reg);
  }

  //
  // Emit mask that causes CALLS instruction to save callee saved registers.
  //

  BuildMI(MBB, MBBI, DL, TII.get(VAX::PROCENTRYMASK))
      .addImm(EntryMask)
      .setMIFlags(MachineInstr::FrameSetup);

  LLVM_DEBUG(dbgs() << "stack size: " << StackSize << "\n");
  LLVM_DEBUG(dbgs() << "entry mask: " << EntryMask << "\n");

}

void VAXFrameLowering::emitEpilogue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {}

bool VAXFrameLowering::spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                                                 MachineBasicBlock::iterator MI,
                                                 ArrayRef<CalleeSavedInfo> CSI,
                                                 const TargetRegisterInfo *TRI)  const {
  return true;
}

bool VAXFrameLowering::restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
                                                   MachineBasicBlock::iterator MI,
                                                   MutableArrayRef<CalleeSavedInfo> CSI,
                                                   const TargetRegisterInfo *TRI) const {
  return true;
}
