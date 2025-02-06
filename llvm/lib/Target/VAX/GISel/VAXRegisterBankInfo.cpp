//===- VAXRegisterBankInfo.cpp --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This file implements the targeting of the RegisterBankInfo class for VAX
//===----------------------------------------------------------------------===//

#include "VAXRegisterBankInfo.h"
#include "llvm/ADT/Twine.h"

#define DEBUG_TYPE "vax-reg-bank-info"

#define GET_TARGET_REGBANK_IMPL
#include "VAXGenRegisterBank.inc"

using namespace llvm;

VAXRegisterBankInfo::VAXRegisterBankInfo(const TargetRegisterInfo &TRI)
    : VAXGenRegisterBankInfo() {}
