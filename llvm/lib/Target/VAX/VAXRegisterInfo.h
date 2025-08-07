//===-- VAXRegisterInfo.h - VAX Register Information Impl ---*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_VAX_VAXREGISTERINFO_H
#define LLVM_LIB_TARGET_VAX_VAXREGISTERINFO_H

#include "llvm/CodeGen/TargetRegisterInfo.h"
#include <cstdint>

#define GET_REGINFO_HEADER
#include "VAXGenRegisterInfo.inc"

namespace llvm {

struct VAXRegisterInfo : public VAXGenRegisterInfo {
public:
  VAXRegisterInfo();

  /// Code Generation virtual methods...

  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;

  const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;

  bool eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  // Debug information queries.
  Register getFrameRegister(const MachineFunction &MF) const override;
};

} // end namespace llvm

#endif
