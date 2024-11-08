//===-- VAXMCTargetDesc.cpp - VAX Target Descriptions -----------------===//
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

#include "MCTargetDesc/VAXMCTargetDesc.h"
#include "MCTargetDesc/VAXInstPrinter.h"
#include "MCTargetDesc/VAXMCAsmInfo.h"
#include "TargetInfo/VAXTargetInfo.h"
#include "VAXTargetStreamer.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCDwarf.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "VAXGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "VAXGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "VAXGenRegisterInfo.inc"

static MCInstrInfo *createVAXMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitVAXMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createVAXMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitVAXMCRegisterInfo(X, VAX::PC);
  return X;
}

static MCSubtargetInfo *
createVAXMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  return createVAXMCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/ CPU, FS);
}

static MCAsmInfo *createVAXMCAsmInfo(const MCRegisterInfo &MRI,
                                       const Triple &TT,
                                       const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new VAXMCAsmInfo(TT);

  // Initial state of the frame pointer is SP.
  MCCFIInstruction Inst = MCCFIInstruction::cfiDefCfa(nullptr, VAX::SP, 0);
  MAI->addInitialFrameState(Inst);

  return MAI;
}

static MCInstPrinter *createVAXMCInstPrinter(const Triple &T,
                                               unsigned SyntaxVariant,
                                               const MCAsmInfo &MAI,
                                               const MCInstrInfo &MII,
                                               const MCRegisterInfo &MRI) {
  return new VAXInstPrinter(MAI, MII, MRI);
}

VAXTargetStreamer::VAXTargetStreamer(MCStreamer &S) : MCTargetStreamer(S) {}

VAXTargetStreamer::~VAXTargetStreamer() = default;

namespace {

class VAXTargetAsmStreamer : public VAXTargetStreamer {
  formatted_raw_ostream &OS;

public:
  VAXTargetAsmStreamer(MCStreamer &S, formatted_raw_ostream &OS);

  void emitCCTopData(StringRef Name) override;
  void emitCCTopFunction(StringRef Name) override;
  void emitCCBottomData(StringRef Name) override;
  void emitCCBottomFunction(StringRef Name) override;
};

} // end anonymous namespace

VAXTargetAsmStreamer::VAXTargetAsmStreamer(MCStreamer &S,
                                               formatted_raw_ostream &OS)
    : VAXTargetStreamer(S), OS(OS) {}

void VAXTargetAsmStreamer::emitCCTopData(StringRef Name) {
  OS << "\t.cc_top " << Name << ".data," << Name << '\n';
}

void VAXTargetAsmStreamer::emitCCTopFunction(StringRef Name) {
  OS << "\t.cc_top " << Name << ".function," << Name << '\n';
}

void VAXTargetAsmStreamer::emitCCBottomData(StringRef Name) {
  OS << "\t.cc_bottom " << Name << ".data\n";
}

void VAXTargetAsmStreamer::emitCCBottomFunction(StringRef Name) {
  OS << "\t.cc_bottom " << Name << ".function\n";
}

static MCTargetStreamer *createTargetAsmStreamer(MCStreamer &S,
                                                 formatted_raw_ostream &OS,
                                                 MCInstPrinter *InstPrint) {
  return new VAXTargetAsmStreamer(S, OS);
}

static MCTargetStreamer *createNullTargetStreamer(MCStreamer &S) {
  return new VAXTargetStreamer(S);
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeVAXTargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfoFn X(getTheVAXTarget(), createVAXMCAsmInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(getTheVAXTarget(),
                                      createVAXMCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(getTheVAXTarget(),
                                    createVAXMCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(getTheVAXTarget(),
                                          createVAXMCSubtargetInfo);

  // Register the MCInstPrinter
  TargetRegistry::RegisterMCInstPrinter(getTheVAXTarget(),
                                        createVAXMCInstPrinter);

  TargetRegistry::RegisterAsmTargetStreamer(getTheVAXTarget(),
                                            createTargetAsmStreamer);

  TargetRegistry::RegisterNullTargetStreamer(getTheVAXTarget(),
                                             createNullTargetStreamer);
}
