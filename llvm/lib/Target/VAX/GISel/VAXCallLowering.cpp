//===-- VAXCallLowering.cpp - Call lowering for GlobalISel ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements the lowering of LLVM calls to machine code calls for
/// GlobalISel.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_M68K_GLSEL_VAXCALLLOWERING_H
#define LLVM_LIB_TARGET_M68K_GLSEL_VAXCALLLOWERING_H

#include "VAXCallLowering.h"
#include "VAXCallingConv.h"
#include "VAXISelLowering.h"
#include "VAXInstrInfo.h"
#include "VAXSubtarget.h"
#include "VAXTargetMachine.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/GlobalISel/CallLowering.h"
#include "llvm/CodeGen/GlobalISel/IRTranslator.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include <llvm/CodeGen/MachineFrameInfo.h>
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/CodeGen/TargetCallingConv.h"

#define DEBUG_TYPE "vax-call-lowering"

// Not handled
// * split arguments
// * split return

using namespace llvm;

namespace {

struct VAXOutgoingValueAssigner : public CallLowering::OutgoingValueAssigner {
  // For outgoing, fixed and varargs are the same
  VAXOutgoingValueAssigner(CCAssignFn *AssignFn)
      : CallLowering::OutgoingValueAssigner(AssignFn) {}
};

struct VAXIncomingValueAssigner : public CallLowering::IncomingValueAssigner {
  // For outgoing, fixed and varargs are the same
  VAXIncomingValueAssigner(CCAssignFn *AssignFn)
      : CallLowering::IncomingValueAssigner(AssignFn) {}
};

struct VAXIncomingValueHandler : public CallLowering::IncomingValueHandler {
  VAXIncomingValueHandler(MachineIRBuilder &MIRBuilder,
                           MachineRegisterInfo &MRI)
      : CallLowering::IncomingValueHandler(MIRBuilder, MRI) {}

  uint64_t StackUsed;

private:
  void assignValueToReg(Register ValVReg, Register PhysReg,
                        const CCValAssign &VA) override;

  void assignValueToAddress(Register ValVReg, Register Addr, LLT MemTy,
                            const MachinePointerInfo &MPO,
                            const CCValAssign &VA) override;

  Register getStackAddress(uint64_t Size, int64_t Offset,
                           MachinePointerInfo &MPO,
                           ISD::ArgFlagsTy Flags) override;
};

void VAXIncomingValueHandler::assignValueToReg(Register ValVReg,
                                                Register PhysReg,
                                                const CCValAssign &VA) {
  MIRBuilder.getMRI()->addLiveIn(PhysReg);
  MIRBuilder.getMBB().addLiveIn(PhysReg);
  IncomingValueHandler::assignValueToReg(ValVReg, PhysReg, VA);
}

void VAXIncomingValueHandler::assignValueToAddress(
    Register ValVReg, Register Addr, LLT MemTy, const MachinePointerInfo &MPO,
    const CCValAssign &VA) {
  MachineFunction &MF = MIRBuilder.getMF();
  auto *MMO = MF.getMachineMemOperand(MPO, MachineMemOperand::MOLoad, MemTy,
                                      inferAlignFromPtrInfo(MF, MPO));
  MIRBuilder.buildLoad(ValVReg, Addr, *MMO);
}

Register VAXIncomingValueHandler::getStackAddress(uint64_t Size,
                                                   int64_t Offset,
                                                   MachinePointerInfo &MPO,
                                                   ISD::ArgFlagsTy Flags) {
  auto &MFI = MIRBuilder.getMF().getFrameInfo();
  const bool IsImmutable = !Flags.isByVal();
  int FI = MFI.CreateFixedObject(Size, Offset, IsImmutable);
  MPO = MachinePointerInfo::getFixedStack(MIRBuilder.getMF(), FI);

  // Build Frame Index
  llvm::LLT FramePtr = LLT::pointer(
      0, MIRBuilder.getMF().getDataLayout().getPointerSizeInBits());
  MachineInstrBuilder AddrReg = MIRBuilder.buildFrameIndex(FramePtr, FI);
  StackUsed = std::max(StackUsed, Size + Offset);
  return AddrReg.getReg(0);
}

struct VAXFormalArgHandler : public VAXIncomingValueHandler {
  VAXFormalArgHandler(MachineIRBuilder &MIRBuilder, MachineRegisterInfo &MRI)
      : VAXIncomingValueHandler(MIRBuilder, MRI) {}
};

struct VAXCallReturnHandler : public VAXIncomingValueHandler {
  VAXCallReturnHandler(MachineIRBuilder &MIRBuilder, MachineRegisterInfo &MRI,
                    MachineInstrBuilder &MIB)
      : VAXIncomingValueHandler(MIRBuilder, MRI), MIB(MIB) {}

private:
  void assignValueToReg(Register ValVReg, Register PhysReg,
                        const CCValAssign &VA) override;

  MachineInstrBuilder &MIB;
};

void VAXCallReturnHandler::assignValueToReg(Register ValVReg,
                                                   Register PhysReg,
                                                   const CCValAssign &VA) {
  MIB.addUse(PhysReg, RegState::Implicit);
  Register ExtReg = extendRegister(ValVReg, VA);
  MIRBuilder.buildCopy(PhysReg, ExtReg);
}

struct VAXOutgoingArgHandler : public CallLowering::OutgoingValueHandler {
  VAXOutgoingArgHandler(MachineIRBuilder &MIRBuilder, MachineRegisterInfo &MRI,
                         MachineInstrBuilder MIB)
      : OutgoingValueHandler(MIRBuilder, MRI), MIB(MIB),
        DL(MIRBuilder.getMF().getDataLayout()),
        STI(MIRBuilder.getMF().getSubtarget<VAXSubtarget>()) {}

  void assignValueToReg(Register ValVReg, Register PhysReg,
                        const CCValAssign &VA) override {
    MIB.addUse(PhysReg, RegState::Implicit);
    Register ExtReg = extendRegister(ValVReg, VA);
    MIRBuilder.buildCopy(PhysReg, ExtReg);
  }

  void assignValueToAddress(Register ValVReg, Register Addr, LLT MemTy,
                            const MachinePointerInfo &MPO,
                            const CCValAssign &VA) override {

    // NOTYET i64, i128 - use pseudo ops or directly generate MOVO/MOVQ

    MIRBuilder.buildInstr(VAX::pushl)
        .addReg(ValVReg)
        .setMIFlag(MachineInstr::FrameSetup);
  }



  Register getStackAddress(uint64_t Size, int64_t Offset,
                           MachinePointerInfo &MPO,
                           ISD::ArgFlagsTy Flags) override {
    return Register();
  }
  MachineInstrBuilder MIB;
  const DataLayout &DL;
  const VAXSubtarget &STI;
};

} // end anonymous namespace;

VAXCallLowering::VAXCallLowering(const VAXTargetLowering &TLI)
    : CallLowering(&TLI) {}

bool VAXCallLowering::lowerReturn(MachineIRBuilder &MIRBuilder,
                                  const Value *Val, ArrayRef<Register> VRegs,
                                  FunctionLoweringInfo &FLI,
                                  Register SwiftErrorVReg) const {
  auto MIB = MIRBuilder.buildInstrNoInsert(VAX::ret);
  bool Success = true;
  MachineFunction &MF = MIRBuilder.getMF();
  const Function &F = MF.getFunction();
  MachineRegisterInfo &MRI = MF.getRegInfo();
  auto &TLI = *getTLI<VAXTargetLowering>();

  CCAssignFn *AssignFn =
      TLI.getCCAssignFn(F.getCallingConv(), true, F.isVarArg());
  auto &DL = F.getDataLayout();

  if (!VRegs.empty()) {
    SmallVector<ArgInfo, 8> SplitArgs;
    ArgInfo OrigArg{VRegs, Val->getType(), 0};
    setArgFlags(OrigArg, AttributeList::ReturnIndex, DL, F);
    splitToValueTypes(OrigArg, SplitArgs, DL, F.getCallingConv());
    OutgoingValueAssigner ArgAssigner(AssignFn);
    VAXOutgoingArgHandler ArgHandler(MIRBuilder, MRI, MIB);
    Success = determineAndHandleAssignments(ArgHandler, ArgAssigner, SplitArgs,
                                            MIRBuilder, F.getCallingConv(),
                                            F.isVarArg());
  }

  MIRBuilder.insertInstr(MIB);
  return Success;
}

bool VAXCallLowering::lowerFormalArguments(MachineIRBuilder &MIRBuilder,
                                           const Function &F,
                                           ArrayRef<ArrayRef<Register>> VRegs,
                                           FunctionLoweringInfo &FLI) const {
  MachineFunction &MF = MIRBuilder.getMF();
  MachineRegisterInfo &MRI = MF.getRegInfo();
  const auto &DL = F.getDataLayout();
  auto &TLI = *getTLI<VAXTargetLowering>();

  SmallVector<ArgInfo, 8> SplitArgs;
  unsigned I = 0;
  for (const auto &Arg : F.args()) {
    ArgInfo OrigArg{VRegs[I], Arg.getType(), I};
    setArgFlags(OrigArg, I + AttributeList::FirstArgIndex, DL, F);
    splitToValueTypes(OrigArg, SplitArgs, DL, F.getCallingConv());
    ++I;
  }

  CCAssignFn *AssignFn =
      TLI.getCCAssignFn(F.getCallingConv(), false, F.isVarArg());
  IncomingValueAssigner ArgAssigner(AssignFn);
  VAXFormalArgHandler ArgHandler(MIRBuilder, MRI);
  return determineAndHandleAssignments(ArgHandler, ArgAssigner, SplitArgs,
                                       MIRBuilder, F.getCallingConv(),
                                       F.isVarArg());
}

bool VAXCallLowering::lowerCall(MachineIRBuilder &MIRBuilder,
                                CallLoweringInfo &Info) const {
  MachineFunction &MF = MIRBuilder.getMF();
  const Function &F = MF.getFunction();
  MachineRegisterInfo &MRI = MF.getRegInfo();
  const DataLayout &DL = F.getDataLayout();
  const VAXSubtarget &STI = MF.getSubtarget<VAXSubtarget>();
  const VAXRegisterInfo *TRI = STI.getRegisterInfo();
  auto &TLI = *getTLI<VAXTargetLowering>();
  auto &Ctx = MF.getFunction().getContext();
  // Pending address mode work, only supporting passing function name in a
  // register

  Register CallReg;
  if (Info.Callee.isGlobal()) {
    const GlobalValue *GV = Info.Callee.getGlobal();
    auto Ptr = MIRBuilder.buildGlobalValue(
        LLT::pointer(GV->getAddressSpace(), 32), GV);
    CallReg = Ptr.getReg(0);
  } else if (Info.Callee.isReg())
    CallReg = Info.Callee.getReg(); // Already materialized
  else
    llvm_unreachable("unknown calls source");

  // do we need to constrain register?
  unsigned CallOpc = VAX::calls;

  auto MIB = MIRBuilder.buildInstrNoInsert(CallOpc)
                 .addReg(CallReg)
                 .addRegMask(TRI->getCallPreservedMask(MF, Info.CallConv));

  SmallVector<ArgInfo, 8> SplitArgs;
  for (const auto &OrigArg : Info.OrigArgs) {
    splitToValueTypes(OrigArg, SplitArgs, DL, Info.CallConv);
  }
  // Do the actual argument marshalling.
  CCAssignFn *AssignFn =
      TLI.getCCAssignFn(F.getCallingConv(), false, F.isVarArg());
  VAXOutgoingValueAssigner Assigner(AssignFn);
  VAXOutgoingArgHandler Handler(MIRBuilder, MRI, MIB);
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(Info.CallConv,
                 Info.IsVarArg,
                 MF,
                 ArgLocs,
                 Ctx);

  SmallVector<ArgInfo, 8> ReversedArgs(SplitArgs.rbegin(), SplitArgs.rend());
  if (!determineAssignments(Assigner, ReversedArgs, CCInfo) ||
      !handleAssignments(Handler, ReversedArgs, CCInfo, ArgLocs, MIRBuilder))
      return false;

  MIB.addImm(SplitArgs.size());

  // Now we can add the actual call instruction to the correct basic block.
  MIRBuilder.insertInstr(MIB);

#if 0
  if (Info.Callee.isReg())
    MIB->getOperand(1).setReg(constrainOperandRegClass(
        MF, *TRI, MRI, *MF.getSubtarget().getInstrInfo(),
        *MF.getSubtarget().getRegBankInfo(), *MIB, MIB->getDesc(), Info.Callee,
        0));
#endif
  return true;
}

#endif // LLVM_LIB_TARGET_MVAX_GLSEL_VAXCALLLOWERING_H
