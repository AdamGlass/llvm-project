//===- VAXLegalizerInfo.h ----------------------------------------*- C++ -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This file implements the targeting of the Machinelegalizer class for VAX
//===----------------------------------------------------------------------===//

#include "VAXLegalizerInfo.h"
#include "llvm/CodeGen/GlobalISel/GenericMachineInstrs.h"
#include "llvm/CodeGen/GlobalISel/LegalizerHelper.h"
#include "llvm/CodeGen/GlobalISel/LegalizerInfo.h"
#include "llvm/CodeGen/GlobalISel/MIPatternMatch.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/GlobalISel/Utils.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/MathExtras.h"
#include <initializer_list>

#define DEBUG_TYPE "vax-legalinfo"

using namespace llvm;
using namespace LegalizeActions;

VAXLegalizerInfo::VAXLegalizerInfo(const VAXSubtarget &ST) {

  using namespace TargetOpcode;
  const LLT s8 = LLT::scalar(8);
  const LLT s16 = LLT::scalar(16);
  const LLT s32 = LLT::scalar(32);
  const LLT s64 = LLT::scalar(64);
  const LLT s128 = LLT::scalar(128);
  const LLT p0 = LLT::pointer(0, 32);

  std::initializer_list<LLT> ScalarList = {s8, s16, s32, s64, s128};
  std::initializer_list<LLT> SimpleScalarList = {s8, s16, s32};

  getActionDefinitionsBuilder(G_IMPLICIT_DEF).legalFor(ScalarList);

  getActionDefinitionsBuilder({G_ADD, G_SUB, G_MUL, G_UDIV, G_XOR})
      .legalFor(SimpleScalarList);

  getActionDefinitionsBuilder(G_FRAME_INDEX).legalFor({p0});
  getActionDefinitionsBuilder(G_GLOBAL_VALUE).legalFor({p0});

  getActionDefinitionsBuilder({G_STORE, G_LOAD})
      .legalForTypesWithMemDesc({{s8, p0, s8, 8},
                                 {s16, p0, s16, 8},
                                 {s32, p0, s32, 8},
                                 {s64, p0, s64, 8},
                                 {s128, p0, s128, 8},
                                 {p0, p0, s32, 8}})
      // zero extends via MOVZ
      .legalForTypesWithMemDesc({{s16, p0, s8, 8},
                                 {s32, p0, s8, 8},
                                 {s32, p0, s16, 8},
                                 {s64, p0, s64, 8}})
      .clampScalar(0, s8, s128);

  getActionDefinitionsBuilder(G_PTR_ADD)
      .legalFor({{p0, s8},{p0, s16}, {p0, s32}});

  // hacked
  getActionDefinitionsBuilder(G_CONSTANT)
      .legalFor({p0, s8, s16, s32, s64, s128})
      .widenScalarToNextPow2(0);
  //      .clampScalar(0, s8, 32);

  getActionDefinitionsBuilder(G_FCONSTANT)
      .legalFor({s32});

  getActionDefinitionsBuilder(G_ANYEXT)
      .legalFor({{s32, s8}});

  // N.B. there is adwc/subwc but not sure how to exploit
  // N.B. there is emul/ediv but not sure how to exploit

  getLegacyLegalizerInfo().computeTables();
}
