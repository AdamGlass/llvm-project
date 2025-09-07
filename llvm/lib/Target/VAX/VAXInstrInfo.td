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

def VAXRetNode : SDNode<"VAXISD::RETNODE", SDTNone,
                      [SDNPHasChain, SDNPOptInGlue, SDNPMayLoad, SDNPVariadic]>;
def VAXRegMask : SDNode<"VAXISD::PROCENTRYMASK", SDTUnaryOp,
                      [SDNPHasChain, SDNPOptInGlue, SDNPMayLoad, SDNPVariadic]>;

// Operands

// What is the Operand class for?
// ComplexPattern vs. Operand class
// what is the difference between teh operand class and the memory oriented one?
// Difference between operand<32>, and operand<iptr>
//

def imm8: Operand<i32>, ImmLeaf<i32, [{ return Imm >= 0 && Imm < 256; }]>;
def imm32: Operand<i32>;

def amode: Operand<i32>;
def sentinel: Operand<i32>;

//

def vaxop      : Operand<i32>, ComplexPattern<iPTR, 3, "selectVaxOp"> {
  let MIOperandInfo = (ops amode, GPR, imm32);
  let PrintMethod = "printVaxOp";
}
def gi_vaxop : GIComplexOperandMatcher<s32, "selectVAXComplexOperandSrc<32>">, GIComplexPatternEquiv<vaxop>;

// Recognizes signed integers between 1 and 63 (inclusive), fitting in 7 bits
// def posSImm7 : AsmOperandClass<"posSImm7", "isSImm<7>() && ValueIs>=0">;

def BBOp : Operand<OtherVT>;
def VPcDispOp : Operand<iPTR>;
def VPcDispPat : ComplexPattern<iPTR, 2, "selectVPcDispOp", []>;
def VPcAddrOp : Operand<iPTR>;
def VPcAddrPat : ComplexPattern<iPTR, 2, "selectVPcAddrOp", []>;

def s8: LLT;
def s16: LLT;
def s128: LLT;

multiclass VxOperands<ValueType VT, LLT LLT, int Width> {
  def Src : Operand<VT> {
      let MIOperandInfo = (ops amode, GPR, imm32);
      let PrintMethod = "printVaxOp";
  }
  def SrcPat: ComplexPattern<iPTR, 3, "selectVaxOp">;
  def gi_SrcPat : GIComplexOperandMatcher<LLT, 
  !strconcat("selectVAXComplexOperandSrc<", !cast<string>(Width),">")>, GIComplexPatternEquiv<!cast<ComplexPattern>(NAME # "SrcPat")>;

  def Dst: Operand<VT>;
  def DstPat: ComplexPattern<VT, 3, "select" # NAME # "Dst", [add, frameindex, ld]>;
}

defm BVop: VxOperands<i8, s8, 8>;//, GPRB, GPRB, GPRnoPC>;
defm WVop: VxOperands<i16, s16, 16>;//, GPRW, GPRW, GPRnoPC>;
defm LVop: VxOperands<i32, s32, 32>;//, GPRnoPC, GPRnoPC, GPRnoPC>;
defm QVop: VxOperands<i64, s64, 64>;//, QPR, QPR, GPRnoPC>;
defm OVop: VxOperands<i128, s128, 128>;//, OPR, OPR, GPRnoPC>;

// XXX Can we make OpOffset work or do we need to move to base_op, extended op

class VxSize<int num, string id, string mode_prefix, ValueType vt, RegisterClass src, RegisterClass drc, int opOffset, Operand srcop, ComplexPattern srcpat, Operand dstop, ComplexPattern dstpat, dag zero>{
  int Num = num;
  string Id = id;
  ValueType VT = vt;
  RegisterClass SrcRC = src;
  RegisterClass DstRC = drc;
  int OpOffset = opOffset;
  string ModePrefix = mode_prefix;
  Operand SrcOp = srcop;
  Operand DstOp = dstop;
  ComplexPattern SrcPat = srcpat;
  ComplexPattern DstPat = dstpat;
  dag Zero = zero;
}

def VxSize8: VxSize<8, "b", "B", i8, GPRB, GPRB, 0, BVopSrc, BVopSrcPat, BVopDst, BVopDstPat, (i8 0)>;
def VxSize16: VxSize<16, "w", "W", i16, GPRW, GPRW, 0x20, WVopSrc, WVopSrcPat, WVopDst, WVopDstPat, (i16 0)>;
def VxSize32: VxSize<32, "l", "L", i32, GPR, GPRnoPC, 0x40, LVopSrc, LVopSrcPat, LVopDst, LVopDstPat, (i32 0)>;
def VxSize64: VxSize<64, "q", "Q", i64, QPR, QPR, -24, QVopSrc, QVopSrcPat, QVopDst, QVopDstPat, (i64 0)>;
def VxSize128: VxSize<128, "o", "O", i128, OPR, OPR, 0xFCDC, OVopSrc, OVopSrcPat, OVopDst, OVopSrcPat, (i128 0)>;

defvar VxGenSizes = [VxSize8, VxSize16, VxSize32];

//
// Source: VAX Architecture Reference Manual, '87
//
// NOTE: Temporarily treat as load/store architecture to get something running.
//       Work on addressing mode(s) problem with load/store first.

//
// Integer Arithemtic and Logical Instructions
//
multiclass MInstVAX23<bits<16> base_op, SDNode NODE, VxSize SIZE> {

  // <base_op>       <op>B2
  // <base_op>+0x1   <op>B3
  // <base_op>+0x20  <op>W2
  // <base_op>+0x21  <op>W3
  // <base_op>+0x40  <op>L2
  // <base_op>+0x41  <op>L3

    def SIZE.Id # "3" : InstVAX<!add(!add(base_op, SIZE.OpOffset),1),
                          (outs SIZE.DstRC:$dst),
                          (ins SIZE.SrcRC:$src1, SIZE.SrcRC:$src2),
                          !strconcat(NAME, SIZE.Id # "3", " $src1, $src2, $dst"),
                          [(set SIZE.VT:$dst, (NODE SIZE.SrcRC:$src1, SIZE.SrcRC:$src2))]>;

    // let need to define 2 variant but have to get constraints right
}

// ADD, DIV, MUL, SUB, XOR
//foreach SIZE = [] in {
//  defm add: MInstVAX23<0x80, add, SIZE>;
//  defm div: MInstVAX23<0x86, sdiv, SIZE>;
//  defm mul: MInstVAX23<0x84, mul, SIZE>;
//  defm sub: MInstVAX23<0x82, sub, SIZE>;
//  defm xor: MInstVAX23<0x8C, xor, SIZE>;
//}


multiclass MInstVAX23new<bits<16> base_op, SDNode NODE, VxSize SIZE> {

  // <base_op>       <op>B2
  // <base_op>+0x1   <op>B3
  // <base_op>+0x20  <op>W2
  // <base_op>+0x21  <op>W3
  // <base_op>+0x40  <op>L2
  // <base_op>+0x41  <op>L3

    def SIZE.Id # "3" : InstVAX<!add(!add(base_op, SIZE.OpOffset),1),
                          (outs SIZE.DstRC:$dst),
                          (ins SIZE.SrcOp:$src1, SIZE.SrcOp:$src2),
                          !strconcat(NAME, SIZE.Id # "3", " $src1, $src2, $dst"),
                          [(set SIZE.VT:$dst, (NODE SIZE.SrcPat:$src1, SIZE.SrcPat:$src2))]>;

    // let need to define 2 variant but have to get constraints right
}

foreach SIZE = [VxSize8, VxSize16, VxSize32] in {
  defm add: MInstVAX23new<0x80, add, SIZE>;
  defm div: MInstVAX23new<0x86, sdiv, SIZE>;
  defm mul: MInstVAX23new<0x84, mul, SIZE>;
  defm sub: MInstVAX23new<0x82, sub, SIZE>;
  defm xor: MInstVAX23new<0x8C, xor, SIZE>;
}

foreach SIZE = [VxSize8, VxSize16, VxSize32] in {
  def bic # SIZE.Id # "3": InstVAX<!add(0x8B, SIZE.OpOffset),
                                   (outs SIZE.DstRC:$dst),
                                   (ins SIZE.SrcRC:$mask, SIZE.SrcRC:$src),
                                   "bic" # SIZE.Id # "3 $mask, $src, $dst",
                                   [(set SIZE.VT:$dst, (and (not SIZE.SrcRC:$mask),  SIZE.SrcRC:$src))]>;
  // XXX add BIC<SIZE>2

  def bis # SIZE.Id # "3": InstVAX<!add(0x88, SIZE.OpOffset),
                                   (outs SIZE.DstRC:$dst),
                                   (ins SIZE.SrcRC:$mask, SIZE.SrcRC:$src),
                                   "bis" # SIZE.Id # "3 $mask, $src, $dst",
                                   [(set SIZE.VT:$dst, (or SIZE.SrcRC:$mask,  SIZE.SrcRC:$src))]>;
}

// Implementing this is a PIA
//multiclass MInstVAXID<bits<16> base_op, int val, VxSize SIZE> {
//}

//foreach SIZE = [VxSize8, VxSize16, VxSize32] in {
//  defm inc: MInstVAXID<0x94, 1, SIZE>;
//  defm : MInstVAXID<"dec",0x94, -1, SIZE>;
//}


//def ashl : InstVAX<0x78,
//                   (outs VxSize32.DstRC:$dst),
//                   (ins VxSize8.SrcRC:$cnt, VxSize32.SrcRC:$src),
//                   "ashl $cnt, $src, $dst",
//                   [(set VxSize32.VT:$dst, (shl VxSize32.SrcRC:$src, (anyext posSimm7:$cnt)))]>;

// def ashq : InstVAX<0x79,
//                     (outs VxSize64.DstRC:$dst),
//                     (ins VxSize8.SrcRC:$cnt, VxSize64.SrcRC:$src),
//                     "ashq $cnt, $src, $dst",
//                     [(set VxSize64.VT:$dst, (shl VxSize32.SrcRC:$src, (anyext VxSize8.SrcRC:$cnt)))]>;

// CLR - Clear
foreach SIZE = [VxSize8, VxSize16, VxSize32, VxSize64, VxSize128] in {
  defvar base_op = 0x94;
  defvar offset = SIZE.OpOffset;
  def clr # SIZE.Id : InstVAX<!add(base_op, offset),
                        (outs SIZE.DstRC:$dst),
                        (ins),
                        "clr" # SIZE.Id # " $dst",
                        []> {
//                        [(store SIZE.DstRCPat:$dst, 0)]> {
//    let Constraints = "$dst = $dst";
    let mayStore = 1;
    let hasSideEffects = 1;
  }
}

// CVT - Convert
// Truncation
def cvtlw: InstVAX<0xF7,
                        (outs VxSize16.DstRC:$dst),
                        (ins  VxSize32.SrcRC:$src),
                        "cvtlw $src, $dst",
                        [(set VxSize16.VT:$dst, (trunc VxSize32.SrcRC:$src))]>;

def cvtlb: InstVAX<0xF6,
                        (outs VxSize8.DstRC:$dst),
                        (ins  VxSize32.SrcRC:$src),
                        "cvtlw $src, $dst",
                        [(set VxSize8.VT:$dst, (trunc VxSize32.SrcRC:$src))]>;

def cvtwb: InstVAX<0x33,
                        (outs VxSize8.DstRC:$dst),
                        (ins  VxSize16.SrcRC:$src),
                        "cvtlw $src, $dst",
                        [(set VxSize8.VT:$dst, (trunc VxSize16.SrcRC:$src))]>;
// Sign Extend
def cvtwl: InstVAX<0x32,
                        (outs VxSize32.DstRC:$dst),
                        (ins  VxSize16.SrcRC:$src),
                        "cvtlw $src, $dst",
                        [(set VxSize32.VT:$dst, (sext VxSize16.SrcRC:$src))]>;

def cvtbw: InstVAX<0x99,
                        (outs VxSize16.DstRC:$dst),
                        (ins  VxSize8.SrcRC:$src),
                        "cvtlw $src, $dst",
                        [(set VxSize16.VT:$dst, (sext VxSize8.SrcRC:$src))]>;

def cvtbl: InstVAX<0x96,
                        (outs VxSize32.DstRC:$dst),
                        (ins  VxSize8.SrcRC:$src),
                        "cvtlw $src, $dst",
                        [(set VxSize32.VT:$dst, (sext VxSize8.SrcRC:$src))]>;

// MOV - Move
// XXX for MOVQ VxSize64
foreach SIZE = [VxSize8, VxSize16, VxSize32, VxSize64, VxSize128] in {
  def mov # SIZE.Id : InstVAX<!add(0x90, SIZE.OpOffset),
                          (outs SIZE.DstRC:$dst),
                          (ins SIZE.SrcRC:$src),
                          !strconcat("mov" # SIZE.Id, " $src, $dst"),
                          [(set SIZE.VT:$dst, (load SIZE.SrcRC:$src))]>;

  def movs # SIZE.Id : PseudoInstVAX<
                          (outs),
                          (ins SIZE.SrcRC:$val, GPR:$addr),
                          !strconcat("movs" # SIZE.Id, " $val, $addr"),
                          [(store SIZE.SrcRC:$val, GPR:$addr)]>;
}


foreach SIZE = [VxSize8, VxSize16, VxSize32, VxSize128] in {
  def mova # SIZE.Id : InstVAX<!add(0x9e, SIZE.OpOffset),
                          (outs SIZE.DstRC:$dst),
                          (ins GPR:$base, imm32:$offset),
                          !strconcat("mova" # SIZE.Id, " [$base + $offset], $dst"),
                          []>;
}

// NOTE: Until we have real addressing modes

def movax : PseudoInstVAX<
                          (outs GPRnoPC:$dst),
                          (ins imm32:$offset),
                          "movax $offset, $dst",
                          []>;


foreach SIZE = [VxSize8, VxSize16, VxSize32] in {
  // MNEG - Move Negated
  def mneg # SIZE.Id : InstVAX<!add(0x8e, SIZE.OpOffset),
                          (outs SIZE.DstRC:$dst),
                          (ins SIZE.SrcRC:$src),
                          !strconcat("meng" # SIZE.Id, " $src, $dst"),
                          [(set SIZE.VT:$dst, (sub 0, (load SIZE.SrcRC:$src)))]>;
  // MCOM - Move Complement
  def mcom # SIZE.Id : InstVAX<!add(0x92, SIZE.OpOffset),
                          (outs SIZE.DstRC:$dst),
                          (ins SIZE.SrcRC:$src),
                          !strconcat("mcom" # SIZE.Id, " $src, $dst"),
                          [(set SIZE.VT:$dst, (not (load SIZE.SrcRC:$src)))]>;
}

// Zero Extend
def movzwl: InstVAX<0x3C,
                        (outs VxSize32.DstRC:$dst),
                        (ins  VxSize16.SrcRC:$src),
                        "movzlw $src, $dst",
                        [(set VxSize32.VT:$dst, (zext VxSize16.SrcRC:$src))]>;

def movzbw: InstVAX<0x9B,
                        (outs VxSize16.DstRC:$dst),
                        (ins  VxSize8.SrcRC:$src),
                        "movzlw $src, $dst",
                        [(set VxSize16.VT:$dst, (zext VxSize8.SrcRC:$src))]>;

def movzbl: InstVAX<0x9A,
                        (outs VxSize32.DstRC:$dst),
                        (ins  VxSize8.SrcRC:$src),
                        "movzlw $src, $dst",
                        [(set VxSize32.VT:$dst, (zext VxSize8.SrcRC:$src))]>;


multiclass MCmp<VxSize SIZE> {
  let Defs = [PSL] in
  def cmp # SIZE.Id : InstVAX<!add(0x91, SIZE.OpOffset),
                              (outs),
                              (ins SIZE.SrcRC:$src1, SIZE.SrcRC:$src2),
                              "cmp" # SIZE.Id # " $src1, $src2",
                              []>;
  //defvar CMPInstr = !cast<Instruction>("cmp" # SIZE.Id);
  //def : Pat<(setcc SIZE.SrcRC:$lhs, SIZE.SrcRC:$rhs, SETLE),
    //        (CMPInstr $lhs, $rhs)>;

}

foreach SIZE = [VxSize8, VxSize16, VxSize32] in {
  defm: MCmp<SIZE>;
}

//def: Pat<(setcc VxSize8.SrcRC:$lhs, VxSize8.SrcRC:$rhs, SETLE),
//         (cmpb $lhs, $rhs)>;

//multiclass MCmpPat<VxSize SIZE> {
//  def: Pat<(setcc SIZE.SrcRC:$lhs, SIZE.SrcRC:$rhs, SETLE),
//         (cmpb $lhs, $rhs)>;
//}

//defm: MCmpPat<VxSize8>;

//PatM

// foreach SIZE = [VxSize8, VxSize16, VxSize32] in {
//   defvar CmpInstr = !cast<Instruction>("cmp" # SIZE.Id);
//   foreach COND = [SETOEQ, SETOGT, SETOGE, SETOLT, SETOLE, SETONE,
//                   SETO, SETUO, SETUEQ, SETUGT, SETUGE, SETULT,
//                   SETULE, SETUNE, SETEQ, SETGT, SETGE, SETLT,
//                   SETLE, SETNE] in {
//     def : Pat<(setcc SIZE.SrcRC:$lhs, SIZE.SrcRC:$rhs, !cast<CondCode>(COND)),
//               (CmpInstr $lhs, $rhs)>;
//   }
// }

//defm : CmpWithCondPatterns<VxSize16>;
//defm : CmpWithCondPatterns<VxSize32>;

// foreach SIZE = [VxSize8, VxSize16, VxSize32] in {
//   let Defs = [PSL] in
//   def cmp # SIZE.Id: InstVAX<!add(0x91, SIZE.OpOffset),
//                    (outs),
//                    (ins SIZE.SrcRC:$src1, SIZE.SrcRC:$src2),
//                    "cmp" # SIZE.Id # " $src1, $src2",
//                    []>;

//    foreach COND = [SETOEQ, SETOGT, SETOGE, SETOLT, SETOLE, SETONE, SETO  , SETUO , SETUEQ, SETUGT, SETUGE, SETULT, SETULE, SETUNE, SETEQ, SETGT, SETGE, SETLT, SETLE, SETNE] in {
//       def : Pat<(setcc SIZE.SrcRC:$lhs, SIZE.SrcRC:$rhs, !cast<CondCode>(COND)),
//             (!cast<Instruction>("cmp" # SIZE.Id) $lhs, $rhs)>;
//    }
// }



// B - Branch
multiclass MInstVAXBCond<bits<16> op, string COND> {

  def b # COND : InstVAX<op,
                (outs),
                (ins VPcDispOp:$addr),
                "b" # COND # " $addr",
                []>;

}

defm : MInstVAXBCond<0x14, "bgtr">;    // Branch on Greater (signed)
defm : MInstVAXBCond<0x15, "bleq">;    // Branch on Less Than or Equal (signed)
                                       // Branch on Not Equal (signed)
defm : MInstVAXBCond<0x12, "bneq">;    // Branch on Not Equal (unsigned)
                                       // Branch on Equal (signed)
defm : MInstVAXBCond<0x13, "beql">;    // Branch on Equal (unsigned)
defm : MInstVAXBCond<0x18, "bgeq">;    // Branch on Greater Than or Equal (signed)
defm : MInstVAXBCond<0x19, "blss">;    // Branch on Less Than (signed)
defm : MInstVAXBCond<0x1A, "bgtru">;   // Branch on Greater Than (unsigned)
defm : MInstVAXBCond<0x1B, "blequ">;   // Branch on Less Than or Equal (unsigned)
defm : MInstVAXBCond<0x1C, "bvc">;     // Branch on Overflow Clear
defm : MInstVAXBCond<0x1D, "bvs">;     // Branch on Overflow Set
                                       // Branch on Greater than Or Equal
defm : MInstVAXBCond<0x1E, "bcc">;     // Branch on Carry Clear
                                       // Branch on Less Than (unsigned)
defm : MInstVAXBCond<0x1F, "bcs">;     // Branch on Carry Set


// Unconditional branches
def brb: InstVAX<0x11,
                (outs),
                (ins BBOp:$addr),
                "brb $addr",
                []>;

def brw: InstVAX<0x31,
                (outs),
                (ins BBOp:$addr),
                "brw $addr",
                []>;

def jmp: InstVAX<0x17,
                (outs),
                (ins VPcAddrOp:$addr),
                "jmp $addr",
                []>;


def blbs: InstVAX<0xE8,
                 (outs),
                 (ins VxSize8.SrcRC:$src, BBOp:$addr),
                 "blbs $src, $addr",
                []>;

// XXX not working
//def : Pat<(brcond (setne (and VxSize8.SrcRC:$val, (i8 1)), (i8 0)), bb:$target),
//          (blbs $val, $target)>;

// BLB - Branch on Low Bit

// TODO: Use relaxation to lower to brb/brw
def: Pat<(brind VPcAddrPat:$dst), (jmp $dst)>;
def: Pat<(br bb:$dst), (brw $dst)>;

// TODO - can these be patterned?
// ACB - Add Compare and Branch
// AOBLEQ - Add One and Branch Less Than Equal
// AOBLSS - Add One and Branch Less Than Equal
// SOBGEQ - Subtract One and Branch Greater Than Equal
// SOBGTR - Subtract One and Branch Greater Than
// CASE - Case

let Uses = [PSL] in {
  // MOVPSL
  def movpsl: InstVAX<0xDC,
                     (outs VxSize32.DstRC:$dst),
                     (ins),
                     "movpsl $dst",
                     []>;
}


// TODO: Alternate calling convention
// BSB - Branch to Subroutine (how differs from CALLS/CALLG)
// JSB - Jump to Subroutine
// RSB - Return from Subroutine

// Other instructions
// ADAWI - Add Aligned Word Interlocked
// ADDWC - Add with Carry
// CMP - Compare
// EDIV - Extended Divide
// EMUL - Extended Multiply


// PUSHL - Push Long
let hasSideEffects = 1, Uses = [SP], Defs = [SP], mayStore = 1 in {
  def pushl: InstVAX<0xDD,
                     (outs),
                     (ins GPR:$src),
                     "pushl $src",
                     []>;
  foreach SIZE = [VxSize64, VxSize128] in {
    def push # SIZE.Id : PseudoInstVAX<(outs),
                                       (ins SIZE.SrcRC:$src),
                                       "push" # SIZE.Id # " $src",
                                       []>;
  }
}

// ROTL - Rotate Long
// SBWC - Subtract With Carry
// TST - Test


// Address instructions
// MOVA - Move Address (possible load/store)
// PUSHA - Push Address

// BitField
// BIT - Bit Test
// CMP - Compare Field
// EXT - Extract Field
// FF - Find First
// INSV - Insert Field
// BB - Branch on Bit
// BBSC - Branch on Bit (and modify without interlock)
// BBSSI - Branch on Bit Interlocked

//
// Control Instructions
//

//
// Procedure Call Instructions
//

// CALLG - Call Procedure with General Argument List (FORTRAN)
// CALLS - Call Procedure with Stack Argument List
def calls: InstVAX<0xFA,
                   (outs),
                   (ins imm32:$numarg, GPR:$dst),
                   "calls $numarg, $dst",
                   []>;

let isReturn = 1, isTerminator = 1, mayLoad = 1, isBarrier = 1 in {
    def ret : InstVAX<0x04, (outs), (ins), "ret", [(VAXRetNode)]>;
}

let isBarrier = 1, hasSideEffects = 1 in {
  // BPT - Breakpoint
  def bpt: InstVAX<0x03,
                  (outs),
                  (ins),
                  "bpt",
                  []>;

  // NOP - NOP
  def nop: InstVAX<0x01,
                   (outs),
                   (ins),
                   "nop",
                   []>;
}



//
// Miscellaneous Instructions
//

// BICPSW - Bit Clear PSW
// BISPSW - Bit Set PSW

// Index - Compute Index

def index: InstVAX<0xA,
                   (outs VxSize32.DstRC:$indexout),
                   (ins VxSize32.SrcRC:$subscript, VxSize32.SrcRC:$low, VxSize32.SrcRC:$high, VxSize32.SrcRC:$size, VxSize32.SrcRC:$indexin),
                   "index $subscript, $low, $high, $size, $indexin, $indexout",
                   []>;
// Alt Pattern but $high isn't used
//                    [(set VxSize32.VT:$indexout,
//                          (add (mul (sub VxSize32.SrcRC:$subscript,
//                                     VxSize32.SrcRC:$low),
//                                    VxSize32.SrcRC:$size),
//                               VxSize32.SrcRC:$indexin))]>;

// POPR - Pop Registers
// PUSHR - Push Registers

//
// Not covered - Floating Point, Queue instructions
//

// Floating Point

defm FVop: VxOperands<f32, s32, 32>; //, FFPR, FFPR, GPRnoPC>;
defm DVop: VxOperands<f64, s64, 64>;// GPRW, GPRW, GPRnoPC>;
defm GVop: VxOperands<f64, s64, 64>;// GPRnoPC, GPRnoPC, GPRnoPC>;
defm HVop: VxOperands<f128, s128, 128>;// QPR, QPR, GPRnoPC>;

def VxSizeF32: VxSize<32, "f", "F", f32, FFPR, FFPR, 0, FVopSrc, FVopSrcPat, FVopDst, FVopDstPat, (f32 0)>;
def VxSizeD64: VxSize<64, "d", "D",f64, DFPR, DFPR, 0x20, DVopSrc, DVopSrcPat, DVopDst, DVopDstPat, (f64 0)>;
def VxSizeG64: VxSize<64, "g", "G", f64, GFPR, GFPR, 0xFD00, GVopSrc, GVopSrcPat, GVopDst, GVopDstPat, (f64 0)>;
def VxSizeH128: VxSize<128, "h","H", f128, HFPR, HFPR, 0xFD20, HVopSrc, HVopSrcPat, HVopDst, HVopDstPat, (f128 0)>;

foreach SIZE = [VxSizeF32, VxSizeD64, VxSizeG64, VxSizeH128] in {
  defm add: MInstVAX23<0x40, fadd, SIZE>;
  defm div: MInstVAX23<0x46, fdiv, SIZE>; // verify type of div
  defm mul: MInstVAX23<0x44, fmul, SIZE>; // verify type of mul
  defm sub: MInstVAX23<0x42, fsub, SIZE>;
}

// BUG: Ambiguity with VxSizeH128
foreach SIZE = [VxSizeF32, VxSizeD64, VxSizeG64] in {
  def mov # SIZE.Id : InstVAX<!add(0x50, SIZE.OpOffset),
                          (outs SIZE.DstRC:$dst),
                          (ins SIZE.SrcRC:$src),
                          !strconcat("mov" # SIZE.Id, " $src, $dst"),
                          [(set SIZE.VT:$dst, SIZE.SrcRC:$src)]>;
}



// XXX not sure how to resolve ambiguity with integr clr
// CLR - Clear
// CMP - Compare

// CVT - Convert

// XXX VxSize128 didn't work
foreach SIZEF = [VxSizeF32, VxSizeD64, VxSizeG64] in {
  def cvtb # SIZEF.Id:
     InstVAX<!add(0x4c, SIZEF.OpOffset),
             (outs SIZEF.DstRC:$dst),
             (ins VxSize8.SrcRC:$src),
             "cvtb" # SIZEF.Id # " $src, $dst",
             [(set SIZEF.VT:$dst, (uint_to_fp VxSize8.SrcRC:$src))]>;

  def cvtw # SIZEF.Id:
     InstVAX<!add(0x4d, SIZEF.OpOffset),
             (outs SIZEF.DstRC:$dst),
             (ins VxSize16.SrcRC:$src),
             "cvtw" # SIZEF.Id # " $src, $dst",
             [(set SIZEF.VT:$dst, (uint_to_fp VxSize16.SrcRC:$src))]>;
  def cvtl # SIZEF.Id:
     InstVAX<!add(0x4e, SIZEF.OpOffset),
             (outs SIZEF.DstRC:$dst),
             (ins VxSize32.SrcRC:$src),
             "cvtl" # SIZEF.Id # " $src, $dst",
             [(set SIZEF.VT:$dst, (uint_to_fp VxSize32.SrcRC:$src))]>;
}

// XXX need to do cvt{float}{integer}

// EMOD - Extended Multiple and Integerize

// XXX had problems with 0 and the pattern
//, VxSizeD64, VxSizeG64, VxSizeH128
//foreach SIZE = [] in {
  // MNEG - Move Negated
//  def mneg # SIZE.Id : InstVAX<!add(0x52, SIZE.OpOffset),
//                          (outs SIZE.DstRC:$dst),
//                          (ins SIZE.SrcRC:$src),
//                          !strconcat("mneg" # SIZE.Id, " $src, $dst"),
//                          [(set SIZE.VT:$dst, (fsub SIZE.Zero, SIZE.SrcRC:$src))]>;
//}

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


// Patterns
//def : Pat<(i8 imm:$val), (movb imm:$val)>;
//def : Pat<(i16 imm:$val), (movw imm:$val)>;
//def : Pat<(i32 imm:$val), (movl imm:$val)>;
//def : Pat<(f32 fpimm:$val), (movf fpimm:$val)>;
//def : Pat<(i64 imm:$val), (movq imm:$val)>;
//def : Pat<(i128 imm:$val), (movo imm:$val)>;
//def : Pat<(G_ADD VAXComplexOperandSrc:$foo, VAXComplexOperandSrc:$bar),
//    (add $foo, $bar)>;
