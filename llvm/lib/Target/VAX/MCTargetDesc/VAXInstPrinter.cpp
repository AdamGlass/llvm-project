//===-- VAXInstPrinter.cpp - Convert VAX MCInst to assembly syntax ----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class prints an VAX MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "VAXInstPrinter.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <cassert>

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

#include "VAXGenAsmWriter.inc"

void VAXInstPrinter::printRegName(raw_ostream &OS, MCRegister Reg) {
  OS << StringRef(getRegisterName(Reg)).lower();
}

void VAXInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                               StringRef Annot, const MCSubtargetInfo &STI,
                               raw_ostream &O) {
  printInstruction(MI, Address, O);
  printAnnotation(O, Annot);
}

void VAXInstPrinter::printInlineJT(const MCInst *MI, int opNum,
                                   raw_ostream &O) {
  report_fatal_error("can't handle InlineJT");
}

void VAXInstPrinter::printInlineJT32(const MCInst *MI, int opNum,
                                     raw_ostream &O) {
  report_fatal_error("can't handle InlineJT32");
}

static void printExpr(const MCExpr *Expr, const MCAsmInfo *MAI,
                      raw_ostream &OS) {
  int Offset = 0;
  const MCSymbolRefExpr *SRE;

  if (const MCBinaryExpr *BE = dyn_cast<MCBinaryExpr>(Expr)) {
    SRE = dyn_cast<MCSymbolRefExpr>(BE->getLHS());
    const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(BE->getRHS());
    assert(SRE && CE && "Binary expression must be sym+const.");
    Offset = CE->getValue();
  } else {
    SRE = dyn_cast<MCSymbolRefExpr>(Expr);
    assert(SRE && "Unexpected MCExpr type.");
  }

  SRE->getSymbol().print(OS, MAI);

  if (Offset) {
    if (Offset > 0)
      OS << '+';
    OS << Offset;
  }
}

void VAXInstPrinter::printVaxOp(const MCInst *MI, unsigned OpNo,
                                raw_ostream &O) {

  const MCOperand &ModeOp = MI->getOperand(OpNo);
  const MCOperand &RegOp = MI->getOperand(OpNo+1);
  const MCOperand &LitOp = MI->getOperand(OpNo+2);

  O << "VAXOP: ";

  int64_t mode = ModeOp.getImm();

  switch (mode) {
  case 0:
  case 1:
    O << '$' << LitOp.getImm();
    return;
  case 2:
    printRegName(O, RegOp.getReg());
    return;
  }
}

void VAXInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                  raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    printRegName(O, Op.getReg());
    return;
  }

  if (Op.isImm()) {
    O << '$' << Op.getImm();
    return;
  }

  if (Op.isDFPImm()) {
    uint64_t Bits = Op.getDFPImm();
    double Val = bit_cast<double>(Bits);

    O << '$' << Val;
    return;
  }
  assert(Op.isExpr() && "unknown operand kind in printOperand");
  printExpr(Op.getExpr(), &MAI, O);
}
