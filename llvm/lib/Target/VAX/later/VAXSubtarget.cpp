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
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "vax-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "VAXGenSubtargetInfo.inc"

void VAXSubtarget::anchor() { }

VAXSubtarget::VAXSubtarget(const Triple &TT, const std::string &CPU,
                               const std::string &FS, const TargetMachine &TM)
    : VAXGenSubtargetInfo(TT, CPU, /*TuneCPU*/ CPU, FS), FrameLowering(*this),
      TLInfo(TM, *this) {}
