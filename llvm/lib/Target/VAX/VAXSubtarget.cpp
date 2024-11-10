//===-- VAXSubtarget.cpp - VAX Subtarget Information ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the VAX specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "VAXSubtarget.h"
#include "VAX.h"
#include "VAXInstrInfo.h"
#include "VAXRegisterInfo.h"
#include "MCTargetDesc/VAXMCTargetDesc.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/CodeGen/ScheduleDAGInstrs.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetMachine.h"
#include <algorithm>
#include <cassert>
#include <map>
#include <optional>

using namespace llvm;

#define DEBUG_TYPE "xcore-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "VAXGenSubtargetInfo.inc"

void VAXSubtarget::anchor() { }

VAXSubtarget::VAXSubtarget(const Triple &TT, const std::string &CPU,
                               const std::string &FS, const TargetMachine &TM)
    : VAXGenSubtargetInfo(TT, CPU, /*TuneCPU*/ CPU, FS), FrameLowering(*this),
      TLInfo(TM, *this) {}
