//===- VAX.cpp -------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ABIInfoImpl.h"
#include "TargetInfo.h"

using namespace clang;
using namespace clang::CodeGen;

//===----------------------------------------------------------------------===//
// VAX ABI Implementation.
//
namespace {
class VAXABIInfo : public DefaultABIInfo {
public:
  VAXABIInfo(CodeGenTypes &CGT) : DefaultABIInfo(CGT) {}

#if 0
private:
  ABIArgInfo classifyReturnType(QualType RetTy) const;
  ABIArgInfo classifyArgumentType(QualType RetTy) const;
  void computeInfo(CGFunctionInfo &FI) const override;
#endif
};
} // end anonymous namespace

namespace {
class VAXTargetCodeGenInfo : public TargetCodeGenInfo {
public:
  VAXTargetCodeGenInfo(CodeGenTypes &CGT)
      : TargetCodeGenInfo(std::make_unique<VAXABIInfo>(CGT)) {}
};
} // end anonymous namespace

std::unique_ptr<TargetCodeGenInfo>
CodeGen::createVAXTargetCodeGenInfo(CodeGenModule &CGM) {
  return std::make_unique<VAXTargetCodeGenInfo>(CGM.getTypes());
}
