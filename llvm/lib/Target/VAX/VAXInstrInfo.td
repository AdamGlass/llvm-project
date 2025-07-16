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
  def Src: Operand<VT> {
    let MIOperandInfo = (ops SRC:$reg, MRC:$mreg);
  }
  def Dst: Operand<VT> {
    let MIOperandInfo = (ops DRC:$reg,  MRC:$mreg);
  }

  def SrcPat: ComplexPattern<VT, 2, "select" # NAME # "Src", []>;
  def DstPat: ComplexPattern<VT, 2, "select" # NAME # "Dst", []>;
}

defm BVop: VxOperands<i8, GPRB, GPRB, GPRnoPC>;
defm WVop: VxOperands<i16, GPRW, GPRW, GPRnoPC>;
defm LVop: VxOperands<i32, GPRnoPC, GPRnoPC, GPRnoPC>;
defm QVop: VxOperands<i64, QPR, QPR, GPRnoPC>;
defm OVop: VxOperands<i128, OPR, OPR, GPRnoPC>;

class VxSize<int num, string id, string mode_prefix, ValueType vt, RegisterClass src, RegisterClass drc, int opOffset, Operand srcop, ComplexPattern srcpat, Operand dstop, ComplexPattern dstpat>{
  int Num = num;
  string Id = id;
  ValueType VT = vt;
  RegisterClass SRC = src;
  RegisterClass DRC = drc;
  int OpOffset = opOffset;
  string ModePrefix = mode_prefix;
  Operand SrcOp = srcop;
  Operand DstOp = dstop;
  ComplexPattern SrcOpPat = srcpat;
  ComplexPattern DstOpPat = dstpat;
}

def VxSize8: VxSize<8, "b", "B", i8, GPRB, GPRB, 0, BVopSrc, BVopSrcPat, BVopDst, BVopDstPat>;
def VxSize16: VxSize<16, "w", "W", i16, GPRW, GPRW, 0x20, WVopSrc, WVopSrcPat, WVopDst, WVopDstPat>;
def VxSize32: VxSize<32, "l", "L", i32, GPR, GPRnoPC, 0x40, LVopSrc, LVopSrcPat, LVopDst, LVopDstPat>;
def VxSize64: VxSize<64, "q", "Q", i64, QPR, QPR, 0x40, QVopSrc, QVopSrcPat, QVopDst, QVopDstPat>;
def VxSize128: VxSize<128, "o", "O", i128, OPR, OPR, 0, OVopSrc, OVopSrcPat, OVopDst, OVopSrcPat>;

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
multiclass MInstVAXINew<bits<16> base_op, SDNode NODE, VxSize SIZE> {

    def SIZE.Id # "3" : InstVAX<!add(!add(base_op, SIZE.OpOffset),1),
                          (outs SIZE.DstOp:$dst),
                          (ins SIZE.SrcOp:$src1, SIZE.SrcOp:$src2),
                          !strconcat(NAME, SIZE.Id # "3", " $src1, $src2, $dst"),
                          [(set SIZE.VT:$dst, (NODE SIZE.SrcOpPat:$src1, SIZE.SrcOpPat:$src2))]>;

    // let need to define 2 variant but have to get constraints right
}

foreach SIZE = [VxSize8, VxSize16, VxSize32] in {
  defm add: MInstVAXINew<0x80, add, SIZE>;
//  defm bic: MInstVAXI<0x8A, bic, SIZE>;
//  defm bis: MInstVAXI<0x88, bis, SIZE>;
  defm div: MInstVAXINew<0x86, sdiv, SIZE>;
  defm mul: MInstVAXINew<0x84, mul, SIZE>;
  defm sub: MInstVAXINew<0x82, sub, SIZE>;
  defm xor: MInstVAXINew<0x8C, xor, SIZE>;
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

defm FVop: VxOperands<f32, FFPR, FFPR, GPRnoPC>;
defm DVop: VxOperands<f64, GPRW, GPRW, GPRnoPC>;
defm GVop: VxOperands<f64, GPRnoPC, GPRnoPC, GPRnoPC>;
defm HVop: VxOperands<f128, QPR, QPR, GPRnoPC>;

def VxSizeF32: VxSize<32, "f", "F", f32, FFPR, FFPR, 0, FVopSrc, FVopSrcPat, FVopDst, FVopDstPat>;
def VxSizeD64: VxSize<64, "d", "D",f64, DFPR, DFPR, 0x20, DVopSrc, DVopSrcPat, DVopDst, DVopDstPat>;
def VxSizeG64: VxSize<64, "g", "G", f64, GFPR, GFPR, 0xFD00, GVopSrc, GVopSrcPat, GVopDst, GVopDstPat>;
def VxSizeH128: VxSize<128, "h","H", f128, HFPR, HFPR, 0xFD20, HVopSrc, HVopSrcPat, HVopDst, HVopDstPat>;

// multiclass MInstVAXF<string ONAME, bits<16> base_op, SDNode NODE /*, bits<8> OPCODE */> {

//   // <base_op>              <op>F2
//   // <base_op>+0x1          <op>F3
//   // <base_op>+0x20         <op>D2
//   // <base_op>+0x21         <op>D3
//   // <base_op>      0xFD    <op>G2
//   // <base_op>+0x1  0xFD    <op>G3
//   // <base_op>+0x20 0xFD    <op>H2
//   // <base_op>+0x21 0xFD    <op>H3

//   foreach SIZE = [VxSizeF32, VxSizeD64, VxSizeG64, VxSizeH128] in {

//     def ONAME# "r" # SIZE.Id # "3" : InstVAX<!add(!add(base_op, SIZE.OpOffset),1),
//                           (outs SIZE.DRC:$Rd),
//                           (ins SIZE.SRC:$Rn, SIZE.SRC:$Rm),
//                           !strconcat(ONAME, SIZE.Id # "3", " $Rn, $Rm, $Rd"),
//                           [(set SIZE.VT:$Rd, (NODE SIZE.SRC:$Rn, SIZE.SRC:$Rm))]>;

//     let Constraints = "$Rd = $Rn" in {
//       def ONAME# "r" # SIZE.Id # "2" : InstVAX<!add(base_op, SIZE.OpOffset),
//                           (outs SIZE.DRC:$Rd),
//                           (ins SIZE.SRC:$Rn, SIZE.SRC:$Rm),
//                           !strconcat(ONAME, SIZE.Id # "2", " $Rm, $Rd"),
//                           [(set SIZE.VT:$Rd, (NODE SIZE.SRC:$Rn, SIZE.SRC:$Rm))]>;
//     }

//   }
// }

foreach SIZE = [VxSizeF32, VxSizeD64, VxSizeG64, VxSizeH128] in {
  defm add: MInstVAXINew<0x40, fadd, SIZE>;
  defm div: MInstVAXINew<0x46, fdiv, SIZE>; // verify type of div
  defm mul: MInstVAXINew<0x44, fmul, SIZE>; // verify type of mul
  defm sub: MInstVAXINew<0x42, fsub, SIZE>;
}

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
