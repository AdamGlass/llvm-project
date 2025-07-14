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

// VAX specific DAG Nodes

def VAXRet : SDNode<"VAXISD::RET", SDTNone,
                      [SDNPHasChain, SDNPOptInGlue, SDNPMayLoad, SDNPVariadic]>;
def VAXRegMask : SDNode<"VAXISD::PROCENTRYMASK", SDTUnaryOp,
                      [SDNPHasChain, SDNPOptInGlue, SDNPMayLoad, SDNPVariadic]>;

// Operands

// What is the Operand class for?
// What does MIOperandInfo actually do?
// ComplexPattern vs. Operand class
// what is the difference between teh operand class and the memory oriented one?
// Difference between operand<32>, and operand<iptr>
//


// def MEMrd : Operand<i32> {
//   let MIOperandInfo = (ops GPR, i32imm);
// }

// def ADDRrd : ComplexPattern<i32, 2, "selectADDRrd", [frameindex], []>;

def imm6 : PatLeaf<(imm), [{
    uint32_t value = (uint32_t)N->getZExtValue();
    return value <= 63;
}]>;

multiclass VxOperands<ValueType VT, RegisterClass SRC, RegisterClass DRC, RegisterClass MRC> {

  def _RegOp : Operand<VT> {
    let MIOperandInfo = (ops SRC:$reg);
    let OperandType = "OPERAND_REGISTER";
  }

  def _RegDstOp : Operand<VT> {
    let MIOperandInfo = (ops DRC:$reg);
    let OperandType = "OPERAND_REGISTER";
  }

  def _RegDefOp : Operand<VT> {
    let MIOperandInfo = (ops MRC:$reg);
    let OperandType = "OPERAND_MEMORY";
  }

  def _Literal6Op : Operand<VT> {
    let OperandType = "OPERAND_LITERAL";
  }
}

defm BOps: VxOperands<i8, GPRB, GPRB, GPRnoPC>;
defm WOps: VxOperands<i16, GPRW, GPRW, GPRnoPC>;
defm LOps: VxOperands<i32, GPRnoPC, GPRnoPC, GPRnoPC>;
defm QOps: VxOperands<i64, QPR, QPR, GPRnoPC>;
defm OOps: VxOperands<i128, OPR, OPR, GPRnoPC>;

class VxSize<int num, string id, string mode_prefix, ValueType vt, RegisterClass src, RegisterClass drc, int opOffset, list<Operand> srcops = [], list<Operand> dstops = []> {
  int Num = num;
  string Id = id;
  ValueType VT = vt;
  RegisterClass SRC = src;
  RegisterClass DRC = drc;
  int OpOffset = opOffset;
  string ModePrefix = mode_prefix;
  list<Operand> SrcOperands = srcops;
  list<Operand> DstOperands = dstops;
}

def VxSize8: VxSize<8, "b", "B", i8, GPRB, GPRB, 0,
    [BOps_RegOp, BOps_RegDefOp, BOps_Literal6Op],
    [BOps_RegDstOp, BOps_RegDefOp]>;
def VxSize16: VxSize<16, "w", "W", i16, GPRW, GPRW, 0x20,
    [WOps_RegOp, WOps_RegDefOp, WOps_Literal6Op],
    [WOps_RegDstOp, WOps_RegDefOp]>;
def VxSize32: VxSize<32, "l", "L", i32, GPR, GPRnoPC, 0x40,
    [LOps_RegOp, LOps_RegDefOp, LOps_Literal6Op],
    [LOps_RegDstOp, LOps_RegDefOp]>;
def VxSize64: VxSize<64, "q", "Q", i64, QPR, QPR, 0x40,
    [QOps_RegOp, QOps_RegDefOp, QOps_Literal6Op],
    [QOps_RegDstOp, QOps_RegDefOp]>;
def VxSize128: VxSize<128, "o", "O", i128, OPR, OPR, 0,
    [OOps_RegOp, OOps_RegDefOp, OOps_Literal6Op],
    [OOps_RegDstOp, OOps_RegDefOp]>;

defvar VxGenSizes = [VxSize8, VxSize16, VxSize32];

//
// Source: VAX Architecture Reference Manual, '87
//

//
// Integer Arithemtic and Logical Instructions
//
// CLR - Clear
// CMP - Compare
// CVT - Convert
// DEC - Decrement

// CLR - Clear

multiclass MInstVAXI<string ONAME, bits<16> base_op, SDNode NODE, VxSize SIZE /*, bits<8> OPCODE */> {

  // <base_op>       <op>B2
  // <base_op>+0x1   <op>B3
  // <base_op>+0x20  <op>W2
  // <base_op>+0x21  <op>W3
  // <base_op>+0x40  <op>L2
  // <base_op>+0x41  <op>L3

  foreach OPSRC = [SIZE.SRC] in {
  foreach OPDST = [SIZE.DRC] in {
    def ONAME # "r" # SIZE.Id # "3" : InstVAX<!add(!add(base_op, SIZE.OpOffset),1),
                          (outs SIZE.DRC:$Rd),
                          (ins OPSRC:$Rn, SIZE.SRC:$Rm),
                          !strconcat(ONAME, SIZE.Id # "3", " $Rn, $Rm, $Rd"),
                          [(set SIZE.VT:$Rd, (NODE SIZE.SRC:$Rn, SIZE.SRC:$Rm))]>;

//    let Constraints = "$Rd = $Rn" in {
//      def ONAME # "r" # SIZE.Id # "2" : InstVAX<!add(base_op, SIZE.OpOffset),
//                          (outs SIZE.DRC:$Rd),
//                          (ins OPSRC:$Rn, SIZE.SRC:$Rm),
//                          !strconcat(ONAME, SIZE.Id # "2", " $Rm, $Rd"),
//                          [(set SIZE.VT:$Rd, (NODE SIZE.SRC:$Rn, SIZE.SRC:$Rm))]>;
//    }
  }
  }
}

foreach SIZE = [VxSize8, VxSize16, VxSize32] in {
  defm : MInstVAXI<"add", 0x80, add, SIZE>;
//defm : MInstVAXI<"bic", 0x8A, bic, SIZE>;
//defm : MInstVAXI<"bis", 0x88, bis, SIZE>;
  defm : MInstVAXI<"div", 0x86, sdiv, SIZE>;
  defm : MInstVAXI<"mul", 0x84, mul, SIZE>;
  defm : MInstVAXI<"sub", 0x82, sub, SIZE>;
  defm : MInstVAXI<"xor", 0x8C, xor, SIZE>;
}

// Other instructions
// ADAWI - Add Aligned Word Interlocked
// ADDWC - Add with Carry
// ASH - Arithmetic Shift
// BIT - Bit Test
// CLR - Clear
// CMP - Compare
// CVT - Convert
// DEC - Decrement
// EDIV - Extended Divide
// EMUL - Extended Multiply
// INC - Increment

// MCOM - Move Complement
// MNEG - Move Negated
// MOV - Move
// MOVX - Move Zero-Extended
// PUSHL - Push Long
// ROTL - Rotate Long
// SBWC - Subtract With Carry
// TST - Test

// Address instructions
// MOVA - Move Address (possible load/store)
// PUSHA - Push Address

// BitField
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
    def Ret : InstVAX<0, (outs), (ins), "ret", [(VAXRet)]>;
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

//def MovL2 : InstVAX<(outs GPRnoPC:$rd),
//                    (ins GPR:$rs1),
//                    "movl3 $rs1, $rd",
//                    [(set i32:$rd, i32:$rs1)]>;

// def LDC : InstVAX<200, (outs GPRnoPC:$rd),
//                   (ins i32imm:$i),
//                   "movl2 $i, $rd",
//                   [(set i32:$rd, imm6:$i)]>;
// def LDR : InstVAX<201, (outs GPRnoPC:$Rd),
//                   (ins GPR:$Rs),
//                   "movl2 ($Rs), $Rd",
//                   [(set GPRnoPC:$Rd, (load GPR:$Rs))]>;

// def LDRrd : InstVAX<202, (outs GPRnoPC:$Rd),
//                     (ins MEMrd:$addr),
//                     "movl2 $addr, $Rd",
//                     [(set GPRnoPC:$Rd, (load ADDRrd:$addr))]>;

//def LDRD : InstVAX<(outs GPRnoPC:$Rd),
//                  (ins GPR:$Rs, i32imm:$i),
//                  "movl2 something, $Rd",
//                  [(set GPRnoPC:$Rd, (load (add $Rs $i)))]>

//
// Not covered - Floating Point, Queue instructions
//

// Floating Point

def VxSizeF32: VxSize<32, "f", "Float", f32, FFPR, FFPR, 0>;
def VxSizeD64: VxSize<64, "d", "DFloat",f64, DFPR, DFPR, 0x20>;
def VxSizeG64: VxSize<64, "g", "GFloat", f64, GFPR, GFPR, 0xFD00>;
def VxSizeH128: VxSize<128, "h","HFloat", f128, HFPR, HFPR, 0xFD20>;

multiclass MInstVAXF<string ONAME, bits<16> base_op, SDNode NODE /*, bits<8> OPCODE */> {

  // <base_op>              <op>F2
  // <base_op>+0x1          <op>F3
  // <base_op>+0x20         <op>D2
  // <base_op>+0x21         <op>D3
  // <base_op>      0xFD    <op>G2
  // <base_op>+0x1  0xFD    <op>G3
  // <base_op>+0x20 0xFD    <op>H2
  // <base_op>+0x21 0xFD    <op>H3

  foreach SIZE = [VxSizeF32, VxSizeD64, VxSizeG64, VxSizeH128] in {

    def ONAME# "r" # SIZE.Id # "3" : InstVAX<!add(!add(base_op, SIZE.OpOffset),1),
                          (outs SIZE.DRC:$Rd),
                          (ins SIZE.SRC:$Rn, SIZE.SRC:$Rm),
                          !strconcat(ONAME, SIZE.Id # "3", " $Rn, $Rm, $Rd"),
                          [(set SIZE.VT:$Rd, (NODE SIZE.SRC:$Rn, SIZE.SRC:$Rm))]>;

    let Constraints = "$Rd = $Rn" in {
      def ONAME# "r" # SIZE.Id # "2" : InstVAX<!add(base_op, SIZE.OpOffset),
                          (outs SIZE.DRC:$Rd),
                          (ins SIZE.SRC:$Rn, SIZE.SRC:$Rm),
                          !strconcat(ONAME, SIZE.Id # "2", " $Rm, $Rd"),
                          [(set SIZE.VT:$Rd, (NODE SIZE.SRC:$Rn, SIZE.SRC:$Rm))]>;
    }

  }
}

defm : MInstVAXF<"add", 0x40, fadd>;
defm : MInstVAXF<"div", 0x46, fdiv>; // verify type of div
defm : MInstVAXF<"mul", 0x44, fmul>; // verify type of mul
defm : MInstVAXF<"sub", 0x42, fsub>;


// CLR - Clear
// CMP - Compare
// CVT - Convert
// EMOD - Extended Multiple and Integerize
// MNEG - Move Negated
// MOV - Move
// Poly - Polynomial evaluate
// Tst - Test

// Character Operations (maybe)
// CMPC - Compare Characters
// LOC - Locate Character
// MATCH - Match Character
// MOVC - Move Character
// MOVTC - Move Translated Character
// MOVTUC - Move Translated Until Character
// SCANC - Scan Character
// SKIPC - Skip Character
// SPANC - Span Character

// CRC (maybe)

// Vector operations (extension)

// Packed operations (no)

//
// FAUX Load/Store instructions
//

def PROCENTRYMASK : PseudoInstVAX<(outs),
                       (ins i32imm:$i),
                        ".error",
                       []>;
