//===- VAXInstructionSelector.cpp --------------------------------*- C++ -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This file implements the targeting of the InstructionSelector class for VAX.
//===----------------------------------------------------------------------===//

#include "VAXInstrInfo.h"
#include "VAXRegisterBankInfo.h"
#include "VAXSubtarget.h"
#include "VAXTargetMachine.h"
#include "llvm/CodeGen/GlobalISel/GIMatchTableExecutorImpl.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelector.h"
#include "llvm/CodeGen/GlobalISel/MIPatternMatch.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
//#include "llvm/IR/IntrinsicsVAX.h"

#define DEBUG_TYPE "vax-gisel"

using namespace llvm;
using namespace MIPatternMatch;

enum VAX_AMode {
  VAXAm_IndLiteral = 0,
  VAXAm_ImmLiteral = 1,
  VAXAm_Register = 2,
  VAXAm_RegisterDeferred = 3,
};

namespace {

#define GET_GLOBALISEL_PREDICATE_BITSET
#include "VAXGenGlobalISel.inc"
#undef GET_GLOBALISEL_PREDICATE_BITSET

class VAXInstructionSelector : public InstructionSelector {
public:
  VAXInstructionSelector(const VAXTargetMachine &TM, const VAXSubtarget &STI,
                         const VAXRegisterBankInfo &RBI);

  bool select(MachineInstr &I) override;
  static const char *getName() { return DEBUG_TYPE; }

private:
  /// tblgen generated 'select' implementation that is used as the initial
  /// selector for the patterns that do not require complex C++.
  bool selectImpl(MachineInstr &I, CodeGenCoverage &CoverageInfo) const;

  bool selectFrameIndex(MachineInstr &I, MachineRegisterInfo &MRI,
                        MachineFunction &MF) const;
  bool selectGlobalValue(MachineInstr &I, MachineRegisterInfo &MRI,
                        MachineFunction &MF) const;

  ComplexRendererFns selectVAXComplexOperand(const MachineOperand &Root,
                                             bool Source,
                                             int Width) const;

  template <int Width>
  ComplexRendererFns selectVAXComplexOperandSrc(MachineOperand &Root) const {
    return selectVAXComplexOperand(Root, true, Width);
  }

  bool isCompatibleRC(const TargetRegisterClass &TargetRC, const Register &Reg) const;

  const VAXInstrInfo &TII;
  const VAXRegisterInfo &TRI;
  const VAXRegisterBankInfo &RBI;

#define GET_GLOBALISEL_PREDICATES_DECL
#include "VAXGenGlobalISel.inc"
#undef GET_GLOBALISEL_PREDICATES_DECL

#define GET_GLOBALISEL_TEMPORARIES_DECL
#include "VAXGenGlobalISel.inc"
#undef GET_GLOBALISEL_TEMPORARIES_DECL
};

} // namespace

#define GET_GLOBALISEL_IMPL
#include "VAXGenGlobalISel.inc"
#undef GET_GLOBALISEL_IMPL

VAXInstructionSelector::VAXInstructionSelector(const VAXTargetMachine &TM,
                                               const VAXSubtarget &STI,
                                               const VAXRegisterBankInfo &RBI)
    : TII(*STI.getInstrInfo()), TRI(*STI.getRegisterInfo()), RBI(RBI),
#define GET_GLOBALISEL_PREDICATES_INIT
#include "VAXGenGlobalISel.inc"
#undef GET_GLOBALISEL_PREDICATES_INIT
#define GET_GLOBALISEL_TEMPORARIES_INIT
#include "VAXGenGlobalISel.inc"
#undef GET_GLOBALISEL_TEMPORARIES_INIT
{
}

bool VAXInstructionSelector::select(MachineInstr &I) {

  LLVM_DEBUG(dbgs() << "select("<< I.getOpcode() << ")\n");
  if (!isPreISelGenericOpcode(I.getOpcode()))
    return true;

  if (I.getOpcode() == TargetOpcode::G_CONSTANT) {
    // Let the consumer matcher handle it
    return false;
  }

  if (selectImpl(I, *CoverageInfo))
    return true;

  MachineBasicBlock &MBB = *I.getParent();
  MachineFunction &MF = *MBB.getParent();
  MachineRegisterInfo &MRI = MF.getRegInfo();

  LLVM_DEBUG(dbgs() << "VAX special processing " << I.getOpcode() << "\n");
  switch (I.getOpcode()) {
  default:
    return false;
  case TargetOpcode::G_FRAME_INDEX:
    // case TargetOpcode::G_GEP:
    return selectFrameIndex(I, MRI, MF);
  case TargetOpcode::G_GLOBAL_VALUE:
    return selectGlobalValue(I, MRI, MF);
  }

  return false;
}

// NB. X86 combines this with GEP
bool VAXInstructionSelector::selectFrameIndex(MachineInstr &I,
                                              MachineRegisterInfo &MRI,
                                              MachineFunction &MF) const {
  // Use LEA to calculate frame index and GEP
  I.setDesc(TII.get(VAX::moval));
  MachineInstrBuilder MIB(MF, I);

  I.addOperand(MachineOperand::CreateImm(0));

  return constrainSelectedInstRegOperands(I, TII, TRI, RBI);
}

bool VAXInstructionSelector::selectGlobalValue(MachineInstr &MI,
                                              MachineRegisterInfo &MRI,
                                              MachineFunction &MF) const {
  assert(MI.getOpcode() == TargetOpcode::G_GLOBAL_VALUE);

  Register DstReg = MI.getOperand(0).getReg();
  const GlobalValue *GV = MI.getOperand(1).getGlobal();

  MachineIRBuilder MIB(MI);

  MachineInstr &NewMI =
      *MIB.buildInstr(VAX::movax)
      .addDef(DstReg)
      .addGlobalAddress(GV)
      .getInstr();

  if (!constrainSelectedInstRegOperands(NewMI, TII, TRI, RBI))
    return false;
  else {
    MI.eraseFromParent();
    return true;
  }
}

bool VAXInstructionSelector::isCompatibleRC(const TargetRegisterClass &TargetRC, const Register &Reg) const {
  auto &MRI = MF->getRegInfo();

  if (Reg.isVirtual()) {
    const TargetRegisterClass *RC = MRI.getRegClass(Reg);
    return TRI.getCommonSubClass(RC, &TargetRC);
  } else {
    const TargetRegisterClass *RC = TRI.getMinimalPhysRegClass(Reg);
    return TRI.getCommonSubClass(RC, &TargetRC);
  }
}

InstructionSelector::ComplexRendererFns
VAXInstructionSelector::selectVAXComplexOperand(const MachineOperand &Root,
                                                bool Source,
                                                int Size) const
{
  auto &MRI = MF->getRegInfo();

  VAX_AMode Mode;
  Register Reg = Register();
  int64_t Literal = 0;
  std::optional<ValueAndVReg> LiteralReg;

  LLVM_DEBUG(dbgs() << "select VAXOP\n");
  LLVM_DEBUG(dbgs() << "past regclass\n");

  if (Root.isImm()) {
    Literal = Root.getImm();
    Reg = Register();
    Mode = VAXAm_ImmLiteral;
    LLVM_DEBUG(dbgs() << "found immediate\n");
  } else if (!Root.isReg()) {
    LLVM_DEBUG(dbgs() << "not a reg\n");
    return std::nullopt;
  }

  Reg = Root.getReg();
  Mode = VAXAm_Register;

  if (mi_match(Reg, MRI, m_ICst(Literal))) {
    Mode = VAXAm_IndLiteral;
    Reg = Register();
    LLVM_DEBUG(dbgs() << "matched constant reg");
  }

  return {
          {
              [=](MachineInstrBuilder &MIB) {
                MIB.addImm(static_cast<int64_t>(Mode));
              },
              // Reg
              [=](MachineInstrBuilder &MIB) { MIB.addReg(Reg); },
              // Constant
              [=](MachineInstrBuilder &MIB) { MIB.addImm(Literal); },
          }};
}

namespace llvm {
InstructionSelector *
createVAXInstructionSelector(const VAXTargetMachine &TM,
                             const VAXSubtarget &Subtarget,
                             const VAXRegisterBankInfo &RBI) {
  return new VAXInstructionSelector(TM, Subtarget, RBI);
}
} // namespace llvm
