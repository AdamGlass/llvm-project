//===--- VAX.h - VAX ToolChain Implementations --------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_VAX_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_VAX_H

#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY VAXToolChain : public ToolChain {
public:
  VAXToolChain(const Driver &D, const llvm::Triple &Triple,
                 const llvm::opt::ArgList &Args);
  bool isPICDefault() const override;
  bool isPIEDefault(const llvm::opt::ArgList &Args) const override;
  bool isPICDefaultForced() const override;
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif // LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_VAX_H
