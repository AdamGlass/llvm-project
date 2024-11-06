//===-- VAXInstrInfo.td - Target Description for VAX ---*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes the VAX instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction format superclass.
//===----------------------------------------------------------------------===//

include "VAXInstrFormats.td"

def AddL3 : InstVAX<(outs VRegsOut:$rd),
                    (ins VRegs:$rs1, VRegs:$rs2),
                    "addl3 $rs1, $rs2, $rd",
                    [(set i32:$rd, (add i32:$rs1, i32:$rs2))]>;
