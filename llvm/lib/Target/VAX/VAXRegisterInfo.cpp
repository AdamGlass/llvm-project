//===-- VAXRegisterInfo.cpp - VAX Register Information ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the VAX implementation of the MRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "VAXRegisterInfo.h"
#include "VAX.h"
#include "VAXInstrInfo.h"
#if 0
#include "VAXMachineFunctionInfo.h"
#include "VAXSubtarget.h"
#endif
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

#define DEBUG_TYPE "vax-reg-info"

#define GET_REGINFO_TARGET_DESC
#include "VAXGenRegisterInfo.inc"

VAXRegisterInfo::VAXRegisterInfo()
  : VAXGenRegisterInfo(0) {
}

const MCPhysReg *
VAXRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  static const MCPhysReg CalleeSavedRegs[] = {
    VAX::R6, VAX::R7, VAX::R8, VAX::R9,
    VAX::R10, VAX::R11
  };

  return CalleeSavedRegs;
}


BitVector VAXRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());

  Reserved.set(VAX::AP);
  Reserved.set(VAX::FP);
  Reserved.set(VAX::SP);
  Reserved.set(VAX::PC);
  Reserved.set(VAX::PSL);
  return Reserved;
}

Register VAXRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return VAX::FP;
}
