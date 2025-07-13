//===-- VAXRegisterInfo.td - VAX Register defs ---------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the VAX register file
//===----------------------------------------------------------------------===//

class VAXReg<bits<16> Enc, string n, list<string> altNames = [], list<Register> subregs = []> : Register<n, altNames> {
  let HWEncoding = Enc;
  let Namespace = "VAX";
  let SubRegs = subregs;
}

let Namespace = "VAX" in {
  def gsublo8 : SubRegIndex<8>;
  def gsublo16 : SubRegIndex<16>;
  def dsub_0 : SubRegIndex<32>;
  def dsub_1 : SubRegIndex<32, 32>;
  def qsub_0 : SubRegIndex<32>;
  def qsub_1 : SubRegIndex<32, 32>;
  def qsub_2 : SubRegIndex<32, 64>;
  def qsub_3 : SubRegIndex<32, 96>;
}

foreach i = 0...11 in {
  def R#i#B : VAXReg<!add(i,20), "">;
  def R#i#W : VAXReg<!add(i,20), "">;
}
foreach i = 0...11 in {
  def R#i : VAXReg<i, "r"#i>, DwarfRegNum<[i]> {
      let SubRegs = [!cast<VAXReg>("R"#i#"B"),!cast<VAXReg>("R"#i#"W")];
      let SubRegIndices = [gsublo8, gsublo16];
  } 
}

def AP  : VAXReg<12, "ap">,  DwarfRegNum<[12]>;
def FP  : VAXReg<13, "fp">,  DwarfRegNum<[13]>;
def SP  : VAXReg<14, "sp">,  DwarfRegNum<[14]>;
def PC  : VAXReg<15, "pc">,  DwarfRegNum<[15]>;

def PSL : VAXReg<16, "psl">, DwarfRegNum<[16]>;

def PSC: RegisterClass<"VAX", [i32], 32, (add PSL)>;

def GPR : RegisterClass<"VAX", [i32], 32,
  (add R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, AP, FP, SP, PC)>;
def GPRno : RegisterClass<"VAX", [i32], 32, (add (sub GPR, AP, FP, SP, PC))>;
def GPRnoPC: RegisterClass<"VAX", [i32], 32, (add (sub GPR, PC))>;

def GPRB : RegisterClass<"VAX", [i8], 8,
  (add R0B, R1B, R2B, R3B, R4B, R5B, R6B, R7B, R8B, R9B, R10B, R11B)>;
def GPRW : RegisterClass<"VAX", [i16], 16,
  (add R0W, R1W, R2W, R3W, R4W, R5W, R6W, R7W, R8W, R9W, R10W, R11W)>;

def FFPR : RegisterClass<"VAX", [f32], 32, (add (sub GPR, AP, FP, SP, PC))>;

def DTuples : RegisterTuples<[dsub_0, dsub_1], [(shl GPRno, 0), (shl GPRno, 1)]>;
def QTuples : RegisterTuples<[qsub_0, qsub_1, qsub_2, qsub_3], [(shl GPRno, 0), (shl GPRno, 1), (shl GPRno, 2), (shl GPRno, 3)]>;

def QPR: RegisterClass<"VAX", [i64], 64, (add DTuples)>;
def DFPR: RegisterClass<"VAX", [f64], 64, (add DTuples)>;
def GFPR: RegisterClass<"VAX", [f64], 64, (add DTuples)>;


def OPR: RegisterClass<"VAX", [i128], 128, (add QTuples)>;
def HFPR: RegisterClass<"VAX", [f128], 128, (add QTuples)>;


