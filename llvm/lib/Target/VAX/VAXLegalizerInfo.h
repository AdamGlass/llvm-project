//===- VAXLegalizerInfo.h ----------------------------------------*- C++ -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This file declares the targeting of the Machinelegalizer class for VAX
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VAX_GISEL_VAXMACHINELEGALIZER_H
#define LLVM_LIB_TARGET_VAX_GISEL_VAXMACHINELEGALIZER_H

#include "llvm/CodeGen/GlobalISel/LegalizerInfo.h"

namespace llvm {

class VAXSubtarget;

/// This class provides the information for the VAX target legalizer for
/// GlobalISel.
class VAXLegalizerInfo : public LegalizerInfo {
public:
  VAXLegalizerInfo(const VAXSubtarget &ST);
};
} // namespace llvm
#endif
