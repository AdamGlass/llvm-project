//===--- VAX.cpp - Implement VAX target feature support ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements VAX TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "VAX.h"
#include "clang/Basic/Builtins.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetBuiltins.h"

using namespace clang;
using namespace clang::targets;

static constexpr int NumBuiltins =
    clang::VAX::LastTSBuiltin - Builtin::FirstTSBuiltin;

static constexpr llvm::StringTable BuiltinStrings =
    CLANG_BUILTIN_STR_TABLE_START
#define BUILTIN CLANG_BUILTIN_STR_TABLE
#include "clang/Basic/BuiltinsVAX.def"
    ;

static constexpr auto BuiltinInfos = Builtin::MakeInfos<NumBuiltins>({
#define BUILTIN CLANG_BUILTIN_ENTRY
#include "clang/Basic/BuiltinsVAX.def"
});

void VAXTargetInfo::getTargetDefines(const LangOptions &Opts,
                                     MacroBuilder &Builder) const {
  Builder.defineMacro("__vax__");
}

llvm::SmallVector<Builtin::InfosShard>
VAXTargetInfo::getTargetBuiltins() const {
  return {{&BuiltinStrings, BuiltinInfos}};
}

ArrayRef<const char *> VAXTargetInfo::getGCCRegNames() const {
  static const char *const GCCRegNames[] = {
      "r0", "r1", "r2",  "r3",  "r4", "r5", "r6", "r7",
      "r8", "r9", "r10", "r11", "ap", "fp", "sp", "pc"};
  return llvm::ArrayRef(GCCRegNames);
}
