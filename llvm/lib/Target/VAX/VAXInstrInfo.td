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

def MEMrd : Operand<i32> {
  let MIOperandInfo = (ops VRC, i32imm);
}

def ADDRrd : ComplexPattern<i32, 2, "selectADDRrd", [frameindex], []>;

def imm6 : PatLeaf<(imm), [{
    uint32_t value = (uint32_t)N->getZExtValue();
    return value <= 63;
}]>;

// VAX specific DAG Nodes

def VAXRet : SDNode<"VAXISD::RET", SDTNone,
                      [SDNPHasChain, SDNPOptInGlue, SDNPMayLoad, SDNPVariadic]>;
def VAXRegMask : SDNode<"VAXISD::REGMASK", SDTUnaryOp,
                      [SDNPHasChain, SDNPOptInGlue, SDNPMayLoad, SDNPVariadic]>;

//
// Source: VAX Architecture Reference Manual, '87
//

class VAXSize<int Size> {
      string Name =  !cond(!eq(Size, 8)  :"b",
                           !eq(Size, 16) :"w",
                           !eq(Size, 32) :"l");
}

// 
// Integer Arithemtic and Logical Instructions
//

// ADAWI - Add Aligned Word Interlocked

// ADDWC - Add with Carry
// ASH - Arithmetic Shift
// BIC - Bit Clear
// BIS - Bit Set
// BIT - Bit Test
// CLR - Clear
// CMP - Compare
// CVT - Convert
// DEC - Decrement

// CLR - Clear

multiclass MInstVAXR<string ONAME, SDNode NODE /*, bits<8> OPCODE */> {
  // define other variants

  def ONAME#"l3" : InstVAX<(outs VRCO:$Rd),
                          (ins VRC:$Rn, VRC:$Rm),
                          !strconcat(ONAME, "l3", " $Rn, $Rm, $Rd"),
                          [(set i32:$Rd, (NODE VRC:$Rn, VRC:$Rm))]>;
}

defm : MInstVAXR<"add", add>;
defm : MInstVAXR<"div", sdiv>;
defm : MInstVAXR<"mul", mul>;
defm : MInstVAXR<"sub", xor>;

// EDIV - Extended Divide
// EMUL - Extended Multiply
// INC - Increment
// MCOM - Move Complement
// MNEG - Move Negated

//def MovL2 : InstVAX<(outs VRCO:$rd),
//                    (ins VRC:$rs1),
//                    "movl3 $rs1, $rd",
//                    [(set i32:$rd, i32:$rs1)]>;

def LDC : InstVAX<(outs VRCO:$rd),
                  (ins i32imm:$i),
                  "movl2 $i, $rd",
                  [(set i32:$rd, imm6:$i)]>;
def LDR : InstVAX<(outs VRCO:$Rd),
                  (ins VRC:$Rs),
                  "movl2 ($Rs), $Rd",
                  [(set VRCO:$Rd, (load VRC:$Rs))]>;

def LDRrd : InstVAX<(outs VRCO:$Rd),
                    (ins MEMrd:$addr),
                    "movl2 $addr, $Rd",
                    [(set VRCO:$Rd, (load ADDRrd:$addr))]>;

//def LDRD : InstVAX<(outs VRCO:$Rd),
//                  (ins VRC:$Rs, i32imm:$i),
//                  "movl2 something, $Rd",
//                  [(set VRCO:$Rd, (load (add $Rs $i)))]>

// MOVZ - Move Zero-Extended


// PUSHL - Push Long
// ROTL - Rotate Long
// SBWC - Subtract With Carry

// TST - Test


//
// Address Instructions
//

// MOVA - Move Address (possible load/store)
// PUSHA - Push Address

//
// Variable Length Bitfield operations
//

// CMP - Compare Field
// EXT - Extract Field
// FF - Find First
// INSV - Insert Field

//
// Control Instructions
//

// ACB - Add Compare and Branch
// AOBLEQ - Add One and Branch Less Than Equal
// AOBLSS - Add One and Branch Less Than Equal
// B - Branch on {condition}
// BB - Branch on Bit
// BBSC - Branch on Bit (and modify without interlock)
// BBSSI - Branch on Bit Interlocked
// BLB - Branch on Low Bit
// BR - Branch
// BSB - Branch to Subroutine (how differs from CALLS/CALLG)
// CASE - Case
// JMP - Jump
// JSB - Jump to Subroutine
// RSB - Return from Subroutine
// SOBGEQ - Subtract One and Branch Greater Than Equal
// SOBGTR - Subtract One and Branch Greater Than

//
// Procedure Call Instructions
//

// CALLG - Call Procedure with General Argument List
// CALLS - Call Procedure with Stack Argument List
// RET - return from procedure

let isReturn = 1, isTerminator = 1, mayLoad = 1, isBarrier = 1 in {
    def Ret : InstVAX<(outs), (ins), "ret", [(VAXRet)]>;
}

//
// Miscellaneous Instructions
//

// BICPSW - Bit Clear PSW
// BISPSW - Bit Set PSW
// BPT - Breakpoint
// Index - Compute Index
// MOVPSL - Move from PSL
// NOP - NOP
// POPR - Pop Registers
// PUSHR - Push Registers

//
// Not covered - Floating Point, Queue instructions
//

//
// FAUX Load/Store instructions
//

//def RegMask : PseudoInstVAX<(outs),
//                            (ins i32imm:$i),
//                            "mask $i",
//                            [(VAXRegMask $i)]>;
