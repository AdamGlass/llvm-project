//===-- VAXFrameLowering.h - Frame info for VAX Target ------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_VAX_VAXFRAMELOWERING_H
#define LLVM_LIB_TARGET_VAX_VAXFRAMELOWERING_H

#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class VAXSubtarget;

  class VAXFrameLowering: public TargetFrameLowering {
  public:
    VAXFrameLowering(const VAXSubtarget &STI);

    /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
    /// the function.
    void emitPrologue(MachineFunction &MF,
                      MachineBasicBlock &MBB) const override;
    void emitEpilogue(MachineFunction &MF,
                      MachineBasicBlock &MBB) const override;

  protected:
    bool hasFPImpl(const MachineFunction &MF) const override;
  };
}

#endif
