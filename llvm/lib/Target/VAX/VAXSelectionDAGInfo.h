//===-- VAXSelectionDAGInfo.h - VAX SelectionDAG Info -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the VAX subclass for SelectionDAGTargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VAX_VAXSELECTIONDAGINFO_H
#define LLVM_LIB_TARGET_VAX_VAXSELECTIONDAGINFO_H

#include "llvm/CodeGen/SelectionDAGTargetInfo.h"

namespace llvm {

class VAXSelectionDAGInfo : public SelectionDAGTargetInfo {
};

}
#endif
