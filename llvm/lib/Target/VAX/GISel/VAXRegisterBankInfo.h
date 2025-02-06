//===-- VAXRegisterBankInfo.h -----------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file declares the targeting of the RegisterBankInfo class for VAX.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VAX_GISEL_VAXREGISTERBANKINFO_H
#define LLVM_LIB_TARGET_VAX_GISEL_VAXREGISTERBANKINFO_H

#include "MCTargetDesc/VAXMCTargetDesc.h"
#include "llvm/CodeGen/RegisterBankInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGBANK_DECLARATIONS
#include "VAXGenRegisterBank.inc"

namespace llvm {
class TargetRegisterInfo;

class VAXGenRegisterBankInfo : public RegisterBankInfo {
protected:
#define GET_TARGET_REGBANK_CLASS
#include "VAXGenRegisterBank.inc"
};

class VAXRegisterBankInfo final : public VAXGenRegisterBankInfo {
public:
  VAXRegisterBankInfo(const TargetRegisterInfo &TRI);
};
} // namespace llvm

#endif
