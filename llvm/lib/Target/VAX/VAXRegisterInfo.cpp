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
#include "VAXMachineFunctionInfo.h"
#include "VAXSubtarget.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

#define DEBUG_TYPE "vax-reg-info"

#define GET_REGINFO_TARGET_DESC
#include "VAXGenRegisterInfo.inc"

VAXRegisterInfo::VAXRegisterInfo() : VAXGenRegisterInfo(0) {}

const MCPhysReg *
VAXRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_VAX_SaveList;
}

const uint32_t *VAXRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                                      CallingConv::ID) const {
    return CSR_VAX_RegMask;
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

bool VAXRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                          int SPAdj, unsigned FIOperandNum,
                                          RegScavenger *RS) const {
  assert(SPAdj == 0 && "Unexpected");
  MachineInstr &MI = *II;
  MachineOperand &FrameOp = MI.getOperand(FIOperandNum);
  int FrameIndex = FrameOp.getIndex();

  MachineFunction &MF = *MI.getParent()->getParent();
  const VAXInstrInfo &TII =
      *static_cast<const VAXInstrInfo *>(MF.getSubtarget().getInstrInfo());

  const VAXFrameLowering *TFI = getFrameLowering(MF);
  int Offset = MF.getFrameInfo().getObjectOffset(FrameIndex);
  int StackSize = MF.getFrameInfo().getStackSize();

  LLVM_DEBUG(errs() << "\nFunction         : " << MF.getName() << "\n");
  LLVM_DEBUG(errs() << "<--------->\n");
  LLVM_DEBUG(MI.print(errs()));
  LLVM_DEBUG(errs() << "FrameIndex         : " << FrameIndex << "\n");
  LLVM_DEBUG(errs() << "FrameOffset        : " << Offset << "\n");
  LLVM_DEBUG(errs() << "StackSize          : " << StackSize << "\n");

  Offset += StackSize;

  Register FrameReg = getFrameRegister(MF);
  Register Reg = MI.getOperand(0).getReg();
  assert(VAX::GPRRegClass.contains(Reg) && "Unexpected register operand");

  // Special handling of DBG_VALUE instructions.
  if (MI.isDebugValue()) {
    MI.getOperand(FIOperandNum).ChangeToRegister(FrameReg, false /*isDef*/);
    MI.getOperand(FIOperandNum + 1).ChangeToImmediate(Offset);
    return false;
  }

  int FIOffset;
  Register BasePtr;
  FIOffset = TFI->getFrameIndexReference(MF, FrameIndex, BasePtr).getFixed();

  // fold constant into offset.
  Offset += MI.getOperand(FIOperandNum + 1).getImm();
  MI.getOperand(FIOperandNum + 1).ChangeToImmediate(Offset + FIOffset);

  MI.getOperand(FIOperandNum).ChangeToRegister(FrameReg, false);
  return false;
}
