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

class VAXReg<bits<16> Enc, string n, list<string> altNames = [] > : Register<n, altNames> {
  let HWEncoding = Enc;
  let Namespace = "VAX";
}

def R0  : VAXReg< 0, "r0">,  DwarfRegNum<[0]>;
def R1  : VAXReg< 1, "r1">,  DwarfRegNum<[1]>;
def R2  : VAXReg< 2, "r2">,  DwarfRegNum<[2]>;
def R3  : VAXReg< 3, "r3">,  DwarfRegNum<[3]>;
def R4  : VAXReg< 4, "r4">,  DwarfRegNum<[4]>;
def R5  : VAXReg< 5, "r5">,  DwarfRegNum<[5]>;
def R6  : VAXReg< 6, "r6">,  DwarfRegNum<[6]>;
def R7  : VAXReg< 7, "r7">,  DwarfRegNum<[7]>;
def R8  : VAXReg< 8, "r8">,  DwarfRegNum<[8]>;
def R9  : VAXReg< 9, "r9">,  DwarfRegNum<[9]>;
def R10 : VAXReg<10, "r10">, DwarfRegNum<[10]>;
def R11 : VAXReg<11, "r11">, DwarfRegNum<[11]>;
def AP  : VAXReg<12, "ap">,  DwarfRegNum<[12]>;
def FP  : VAXReg<13, "fp">,  DwarfRegNum<[13]>;
def SP  : VAXReg<14, "sp">,  DwarfRegNum<[14]>;
def PC  : VAXReg<15, "pc">,  DwarfRegNum<[15]>;

def PSL : VAXReg<15, "psl">,  DwarfRegNum<[16]>;

def VRegs : RegisterClass<"VAX", [i32], 32,
    (add R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, AP, FP, SP, PC, PSL)>;

def SR :  RegisterClass<"VAX", [i32], 32, (add PSL)>;

def VRegsOut :  RegisterClass<"VAX", [i32], 32, (add (sub VRegs, AP, FP, SP, PC, PSL))>;
