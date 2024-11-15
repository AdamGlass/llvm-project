//===-- VAXAsmPrinter.cpp - VAX LLVM assembly writer ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the XAS-format VAX assembly language.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/VAXInstPrinter.h"
#include "TargetInfo/VAXTargetInfo.h"
#include "VAX.h"
#include "VAXInstrInfo.h"
//#include "VAXMCInstLower.h"
#include "VAXSubtarget.h"
#include "VAXTargetMachine.h"
#include "VAXTargetStreamer.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include <algorithm>
#include <cctype>
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {
  class VAXAsmPrinter : public AsmPrinter {
    //VAXMCInstLower MCInstLowering;
    VAXTargetStreamer &getTargetStreamer();

  public:
    explicit VAXAsmPrinter(TargetMachine &TM,
                             std::unique_ptr<MCStreamer> Streamer)
        : AsmPrinter(TM, std::move(Streamer)) {}
          //MCInstLowering(*this) {}

    StringRef getPassName() const override { return "VAX Assembly Printer"; }

    void emitInstruction(const MachineInstr *MI) override;

#if 0
    bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                         const char *ExtraCode, raw_ostream &O) override;
    bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNum,
                               const char *ExtraCode, raw_ostream &O) override;

    void emitGlobalVariable(const GlobalVariable *GV) override;

    void emitFunctionEntryLabel() override;
    void emitFunctionBodyStart() override;
    void emitFunctionBodyEnd() override;
#endif
  };
} // end of anonymous namespace

VAXTargetStreamer &VAXAsmPrinter::getTargetStreamer() {
  return static_cast<VAXTargetStreamer&>(*OutStreamer->getTargetStreamer());
}

void VAXAsmPrinter::emitInstruction(const MachineInstr *MI) {

  SmallString<128> Str;
  raw_svector_ostream O(Str);

  O << "RAW INSTRUCTION";
  OutStreamer->emitRawText(O.str());
  //  EmitToStreamer(*OutStreamer, TmpInst);
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeVAXAsmPrinter() {
  RegisterAsmPrinter<VAXAsmPrinter> X(getTheVAXTarget());
}
