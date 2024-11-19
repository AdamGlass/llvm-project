//===-- VAXISelLowering.cpp - VAX DAG Lowering Implementation ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the VAXTargetLowering class.
//
//===----------------------------------------------------------------------===//

#include "VAXISelLowering.h"
#include "VAX.h"
#include "VAXMachineFunctionInfo.h"
#include "VAXSubtarget.h"
#include "VAXTargetMachine.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Intrinsics.h"
//#include "llvm/IR/IntrinsicsVAX.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/KnownBits.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>

using namespace llvm;

#define DEBUG_TYPE "vax-lower"

VAXTargetLowering::VAXTargetLowering(const TargetMachine &TM,
                                     const VAXSubtarget &Subtarget)
    : TargetLowering(TM), Subtarget(&Subtarget) {

  // Set up the register classes.
  addRegisterClass(MVT::i32, &VAX::VRegsRegClass);

  // Compute derived properties from the register classes
  computeRegisterProperties(Subtarget.getRegisterInfo());

  setStackPointerRegisterToSaveRestore(VAX::SP);

  // Use i32 for setcc operations results
  setBooleanContents(ZeroOrOneBooleanContent);

  MaxStoresPerMemset = MaxStoresPerMemsetOptSize = 4;
  MaxStoresPerMemmove = MaxStoresPerMemmoveOptSize
    = MaxStoresPerMemcpy = MaxStoresPerMemcpyOptSize = 2;

  setMinFunctionAlignment(Align(4));
  setPrefFunctionAlignment(Align(4));

  // This target doesn't implement native atomics.
  setMaxAtomicSizeInBitsSupported(0);
}

const char *VAXTargetLowering::
getTargetNodeName(unsigned Opcode) const
{
  switch ((VAXISD::NodeType)Opcode)
  {
    case VAXISD::FIRST_NUMBER      : break;
    case VAXISD::RET               : return "VAXISD::RET";
  }
  return nullptr;
}

//===----------------------------------------------------------------------===//
//                      Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "VAXGenCallingConv.inc"

SDValue VAXTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins,
    const SDLoc &DL, SelectionDAG &DAG,
    SmallVectorImpl<SDValue> &InVals) const {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();
  VAXFunctionInfo *VFI = MF.getInfo<VAXFunctionInfo>();

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs,
                 *DAG.getContext());

  CCInfo.AnalyzeFormalArguments(Ins, CC_VAX);

  if (isVarArg) {
    report_fatal_error("Var arg not supported by LowerFormalArguments");
  }

  //
  // All arguments are on stack.
  //
  // XXX not sure how we reference off AP but that maybe later when we lower
  //     the frame index
  //

  //
  // TODO: struct as return (observed)
  //       Caller: stores pointer to reserved area is stored in r1 by caller
  //       Callee: set r0 to r1 value
  //               value is stored at r1's value....
  //

  for (const CCValAssign &VA : ArgLocs) {
      assert(VA.isMemLoc());

      unsigned ObjSize = VA.getLocVT().getSizeInBits() / 8;

      // Is there an issue if the argument is big eg. quad?

      // Create the frame index object for this incoming parameter...
      int FI = MFI.CreateFixedObject(ObjSize, VA.getLocMemOffset(), true);
      // Create the SelectionDAG nodes corresponding to a load
      // from this parameter
      SDValue FIN = DAG.getFrameIndex(FI, MVT::i32);
      InVals.push_back(DAG.getLoad(
          VA.getLocVT(), DL, Chain, FIN,
          MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), FI)));
  }
  return Chain;
}

SDValue
VAXTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                 bool IsVarArg,
                                 const SmallVectorImpl<ISD::OutputArg> &Outs,
                                 const SmallVectorImpl<SDValue> &OutVals,
                                 const SDLoc &DL, SelectionDAG &DAG) const {
  // CCValAssign - represent the assignment of the return value to a location
  SmallVector<CCValAssign, 16> RVLocs;
  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  // Analize return values.
  CCInfo.AnalyzeReturn(Outs, RetCC_VAX);

  SDValue Glue;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i], Glue);

    // Guarantee that all emitted copies are stuck together with flags.
    Glue = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

#if NOTYET
  // The VAX ABI for returning structs by value requires that we copy
  // the sret argument into rv for the return. We saved the argument into
  // a virtual register in the entry block, so now we copy the value out
  // and into rv.
  if (DAG.getMachineFunction().getFunction().hasStructRetAttr()) {
    MachineFunction &MF = DAG.getMachineFunction();
    VAXMachineFunctionInfo *VAXMFI = MF.getInfo<VAXMachineFunctionInfo>();
    Register Reg = VAXMFI->getSRetReturnReg();
    assert(Reg &&
           "SRetReturnReg should have been set in LowerFormalArguments().");
    SDValue Val =
        DAG.getCopyFromReg(Chain, DL, Reg, getPointerTy(DAG.getDataLayout()));

    Chain = DAG.getCopyToReg(Chain, DL, VAX::R0, Val, Glue);
    Glue = Chain.getValue(1);
    RetOps.push_back(
        DAG.getRegister(VAX::R0, getPointerTy(DAG.getDataLayout())));
  }
#endif

  RetOps[0] = Chain; // Update chain

  unsigned Opc = VAXISD::RET;
  if (Glue.getNode())
    RetOps.push_back(Glue);

  // Return Void
  return DAG.getNode(Opc, DL, MVT::Other,
                     ArrayRef<SDValue>(&RetOps[0], RetOps.size()));
}

//===----------------------------------------------------------------------===//
//                           VAX Inline Assembly Support
//===----------------------------------------------------------------------===//

