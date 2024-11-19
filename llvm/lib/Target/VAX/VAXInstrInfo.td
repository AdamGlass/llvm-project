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

//
// Source: VAX Architecture Reference Manual, '87
//

// 
// Integer Arithemtic and Logical Instructions
//

// ADAWI - Add Aligned Word Interlocked

def AddL3 : InstVAX<(outs VRegsOut:$rd),
                    (ins VRegs:$rs1, VRegs:$rs2),
                    "addl3 $rs1, $rs2, $rd",
                    [(set i32:$rd, (add i32:$rs1, i32:$rs2))]>;

// ADDWC - Add with Carry
// ASH - Arithmetic Shift
// BIC - Bit Clear
// BIS - Bit Set
// BIT - Bit Test
// CLR - Clear
// CMP - Compare
// CVT - Convert
// DEC - Decrement

def DivL3 : InstVAX<(outs VRegsOut:$rd),
                    (ins VRegs:$rs1, VRegs:$rs2),
                    "divl3 $rs1, $rs2, $rd",
                    [(set i32:$rd, (sdiv i32:$rs1, i32:$rs2))]>;

// EDIV - Extended Divide
// EMUL - Extended Multiply
// INC - Increment
// MCOM - Move Complement
// MNEG - Move Negated

def MovL2 : InstVAX<(outs VRegsOut:$rd),
                    (ins VRegs:$rs1),
                    "movl3 $rs1, $rd",
                    [(set i32:$rd, i32:$rs1)]>;

// MOVZ - Move Zero-Extended

def MulL3 : InstVAX<(outs VRegsOut:$rd),
                    (ins VRegs:$rs1, VRegs:$rs2),
                    "mull3 $rs1, $rs2, $rd",
                    [(set i32:$rd, (mul i32:$rs1, i32:$rs2))]>;

// PUSHL - Push Long
// ROTL - Rotate Long
// SBWC - Subtract With Carry

def SubL3 : InstVAX<(outs VRegsOut:$rd),
                    (ins VRegs:$rs1, VRegs:$rs2),
                    "subl3 $rs1, $rs2, $rd",
                    [(set i32:$rd, (sub i32:$rs1, i32:$rs2))]>;

// TST - Test

def XorL3 : InstVAX<(outs VRegsOut:$rd),
                    (ins VRegs:$rs1, VRegs:$rs2),
                    "xorl3 $rs1, $rs2, $rd",
                    [(set i32:$rd, (xor i32:$rs1, i32:$rs2))]>;

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

// VAX specific DAG Nodes

def VAXRet : SDNode<"VAXISD::RET", SDTBrind,
                      [SDNPHasChain, SDNPOptInGlue, SDNPMayLoad, SDNPVariadic]>;
