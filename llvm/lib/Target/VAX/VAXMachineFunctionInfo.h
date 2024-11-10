//===- VAXMachineFunctionInfo.h - VAX machine function info -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares VAX-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VAX_VAXMACHINEFUNCTIONINFO_H
#define LLVM_LIB_TARGET_VAX_VAXMACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include <cassert>
#include <utility>
#include <vector>

namespace llvm {

/// VAXFunctionInfo - This class is derived from MachineFunction private
/// VAX target-specific information for each MachineFunction.
class VAXFunctionInfo : public MachineFunctionInfo {
  virtual void anchor();

public:
  VAXFunctionInfo() = default;

  explicit VAXFunctionInfo(const Function &F,
                             const TargetSubtargetInfo *STI) {}

  MachineFunctionInfo *
  clone(BumpPtrAllocator &Allocator, MachineFunction &DestMF,
        const DenseMap<MachineBasicBlock *, MachineBasicBlock *> &Src2DstMBB)
      const override;

  ~VAXFunctionInfo() override = default;

};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_VAX_VAXMACHINEFUNCTIONINFO_H
