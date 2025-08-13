//===-- VAXMCInstLower.cpp - Convert VAX MachineInstr to MCInst -------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains code to lower VAX MachineInstrs to their
/// corresponding MCInst records.
///
//===----------------------------------------------------------------------===//
#include "VAXMCInstLower.h"

#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"

using namespace llvm;

VAXMCInstLower::VAXMCInstLower(MCContext &ctx, AsmPrinter &asmprinter)
    : Ctx(ctx), Printer(asmprinter) {}

MCOperand VAXMCInstLower::LowerSymbolOperand(const MachineOperand &MO,
                                             const MCSymbol *Symbol) const {

  const MCSymbolRefExpr *MCSym = MCSymbolRefExpr::create(Symbol, Ctx);
  return MCOperand::createExpr(MCSym);

#if 0
  if (!Offset)

  // Assume offset is never negative.
  assert(Offset > 0);

  const MCConstantExpr *OffsetExpr =  MCConstantExpr::create(Offset, *Ctx);
  const MCBinaryExpr *Add = MCBinaryExpr::createAdd(MCSym, OffsetExpr, *Ctx);
  return MCOperand::createExpr(Add);
#endif
}

MCOperand VAXMCInstLower::LowerOperand(const MachineOperand &MO) const {
  MachineOperandType MOTy = MO.getType();

  switch (MOTy) {
  default:
    MO.dump();
    llvm_unreachable("unknown operand type");
  case MachineOperand::MO_Register:
    // Ignore all implicit register operands.
    if (MO.isImplicit())
      return MCOperand();
    return MCOperand::createReg(MO.getReg());
  case MachineOperand::MO_Immediate:
    return MCOperand::createImm(MO.getImm());
  case MachineOperand::MO_FPImmediate: {
    APFloat Val = MO.getFPImm()->getValueAPF();
    return MCOperand::createDFPImm(bit_cast<uint64_t>(Val.convertToDouble()));
  }
  case MachineOperand::MO_GlobalAddress:
    return LowerSymbolOperand(MO, Printer.getSymbolPreferLocal(*MO.getGlobal()));
  case MachineOperand::MO_ExternalSymbol:
    return LowerSymbolOperand(MO, Printer.GetExternalSymbolSymbol(MO.getSymbolName()));
  case MachineOperand::MO_MachineBasicBlock:
    return LowerSymbolOperand(MO, MO.getMBB()->getSymbol());
  case MachineOperand::MO_JumpTableIndex:
    return LowerSymbolOperand(MO, Printer.GetJTISymbol(MO.getIndex()));
  case MachineOperand::MO_ConstantPoolIndex:
    return LowerSymbolOperand(MO, Printer.GetCPISymbol(MO.getIndex()));
  case MachineOperand::MO_BlockAddress:
    return LowerSymbolOperand(
        MO,
        Printer.GetBlockAddressSymbol(MO.getBlockAddress()));
  case MachineOperand::MO_RegisterMask:
    // Ignore call clobbers
    return MCOperand();
  }
}

void VAXMCInstLower::Lower(const MachineInstr *MI, MCInst &OutMI) const {
  OutMI.setOpcode(MI->getOpcode());

  for (const MachineOperand &MO : MI->operands()) {
    MCOperand MCOp = LowerOperand(MO);

    if (MCOp.isValid())
      OutMI.addOperand(MCOp);
  }
}
