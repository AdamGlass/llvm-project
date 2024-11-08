//===-- VAXMCTargetDesc.h - VAX Target Descriptions ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides VAX specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VAX_MCTARGETDESC_VAXMCTARGETDESC_H
#define LLVM_LIB_TARGET_VAX_MCTARGETDESC_VAXMCTARGETDESC_H

// Defines symbolic names for VAX registers.  This defines a mapping from
// register name to register number.
//
#define GET_REGINFO_ENUM
#include "VAXGenRegisterInfo.inc"

// Defines symbolic names for the VAX instructions.
//
#define GET_INSTRINFO_ENUM
#define GET_INSTRINFO_MC_HELPER_DECLS
#include "VAXGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "VAXGenSubtargetInfo.inc"

#endif // LLVM_LIB_TARGET_VAX_MCTARGETDESC_VAXMCTARGETDESC_H
