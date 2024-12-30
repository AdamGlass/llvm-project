//===-- VAXTargetMachine.cpp - Define TargetMachine for VAX -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#include "VAXTargetMachine.h"
#include "MCTargetDesc/VAXMCTargetDesc.h"
#include "TargetInfo/VAXTargetInfo.h"
#include "VAX.h"
#include "VAXMachineFunctionInfo.h"
#include "VAXTargetTransformInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/CodeGen.h"
#include <optional>

using namespace llvm;

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
  return RM.value_or(Reloc::Static);
}

static CodeModel::Model
getEffectiveVAXCodeModel(std::optional<CodeModel::Model> CM) {
  if (CM) {
    if (*CM != CodeModel::Small && *CM != CodeModel::Large)
      report_fatal_error("Target only supports CodeModel Small or Large");
    return *CM;
  }
  return CodeModel::Small;
}

namespace {
class VAXELFTargetObjectFile : public TargetLoweringObjectFileELF {
  void Initialize(MCContext &Ctx, const TargetMachine &TM) override {
    TargetLoweringObjectFileELF::Initialize(Ctx, TM);
    InitializeELF(TM.Options.UseInitArray);
  }
};
} // namespace

static std::unique_ptr<TargetLoweringObjectFile> createTLOF() {
  return std::make_unique<VAXELFTargetObjectFile>();
}

/// Create an ILP32 architecture model
///
VAXTargetMachine::VAXTargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       std::optional<Reloc::Model> RM,
                                       std::optional<CodeModel::Model> CM,
                                       CodeGenOptLevel OL, bool JIT)
    : CodeGenTargetMachineImpl(
          T, "e-m:e-p:32:32-i1:8:32-i8:8:32-i16:16:32-i64:32-f64:32-a:0:32-n32",
          TT, CPU, FS, Options, getEffectiveRelocModel(RM),
          getEffectiveVAXCodeModel(CM), OL),
      TLOF(createTLOF()),
      Subtarget(TT, std::string(CPU), std::string(FS), *this) {
  initAsmInfo();
}

VAXTargetMachine::~VAXTargetMachine() = default;

namespace {

/// VAX Code Generator Pass Configuration Options.
class VAXPassConfig : public TargetPassConfig {
public:
  VAXPassConfig(VAXTargetMachine &TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {}

  VAXTargetMachine &getVAXTargetMachine() const {
    return getTM<VAXTargetMachine>();
  }

  void addIRPasses() override;
  bool addPreISel() override;
  bool addInstSelector() override;
  void addPreEmitPass() override;
};

} // end anonymous namespace

TargetPassConfig *VAXTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new VAXPassConfig(*this, PM);
}

void VAXPassConfig::addIRPasses() {
  addPass(createAtomicExpandLegacyPass());

  TargetPassConfig::addIRPasses();
}

bool VAXPassConfig::addPreISel() {
  return false;
}

bool VAXPassConfig::addInstSelector() {
  addPass(createVAXISelDag(getVAXTargetMachine(), getOptLevel()));
  return false;
}

void VAXPassConfig::addPreEmitPass() {
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeVAXTarget() {
  RegisterTargetMachine<VAXTargetMachine> X(getTheVAXTarget());
  PassRegistry &PR = *PassRegistry::getPassRegistry();
  initializeVAXDAGToDAGISelLegacyPass(PR);
}

MachineFunctionInfo *VAXTargetMachine::createMachineFunctionInfo(
    BumpPtrAllocator &Allocator, const Function &F,
    const TargetSubtargetInfo *STI) const {
  return VAXFunctionInfo::create<VAXFunctionInfo>(Allocator, F, STI);
}
