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
  VAXFunctionInfo *XFI = MF.getInfo<VAXFunctionInfo>();

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

//===----------------------------------------------------------------------===//
//                           VAX Inline Assembly Support
//===----------------------------------------------------------------------===//

