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

#define DEBUG_TYPE "vax-legalinfo"

using namespace llvm;
using namespace LegalizeActions;

VAXLegalizerInfo::VAXLegalizerInfo(const VAXSubtarget &ST) {

  using namespace TargetOpcode;
  const LLT s8 = LLT::scalar(8);
  const LLT s16 = LLT::scalar(16);
  const LLT s32 = LLT::scalar(32);
  const LLT p0 = LLT::pointer(0, 32);

  getActionDefinitionsBuilder({G_ADD, G_SUB, G_MUL, G_UDIV})
      .legalFor({s8, s16, s32});

  getActionDefinitionsBuilder(G_XOR)
      .legalFor({s8, s16, s32});

  getActionDefinitionsBuilder({G_SHL, G_ASHR})
      .legalFor({s32});

  getActionDefinitionsBuilder({G_ROTR, G_ROTL})
      .legalFor({s32});

  getActionDefinitionsBuilder({G_FRAME_INDEX, G_GLOBAL_VALUE}).legalFor({p0});


  getActionDefinitionsBuilder({G_STORE, G_LOAD})
      .legalForTypesWithMemDesc({{s32, p0, s32, 4},
                                 {s32, p0, s16, 4},
                                 {s32, p0, s8, 4},
                                 {s16, p0, s16, 2},
                                 {s8, p0, s8, 1},
                                 {p0, p0, s32, 4}})
      .clampScalar(0, s8, s32);

  getActionDefinitionsBuilder(G_PTR_ADD).legalFor({{p0, s32}});

  // N.B. there is adwc/subwc but not sure how to exploit
  // N.B. there is emul/ediv but not sure how to exploit

  getLegacyLegalizerInfo().computeTables();
}
