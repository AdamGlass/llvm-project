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

class GR<bits<16> Enc, string n, list<string> altNames = [] > : Register<n, altNames> {
  let HWEncoding = Enc;
  let Namespace = "VAX";
}

def R0  : GR< 0, "r0">,  DwarfRegNum<[0]>;
def R1  : GR< 1, "r1">,  DwarfRegNum<[1]>;
def R2  : GR< 2, "r2">,  DwarfRegNum<[2]>;
def R3  : GR< 3, "r3">,  DwarfRegNum<[3]>;
def R4  : GR< 4, "r4">,  DwarfRegNum<[4]>;
def R5  : GR< 5, "r5">,  DwarfRegNum<[5]>;
def R6  : GR< 6, "r6">,  DwarfRegNum<[6]>;
def R7  : GR< 7, "r7">,  DwarfRegNum<[7]>;
def R8  : GR< 8, "r8">,  DwarfRegNum<[8]>;
def R9  : GR< 9, "r9">,  DwarfRegNum<[9]>;
def R10 : GR<10, "r10">, DwarfRegNum<[10]>;
def R11 : GR<11, "r11">, DwarfRegNum<[11]>;
def AP  : GR<12, "ap">,  DwarfRegNum<[12]>;
def FP  : GR<13, "fp">,  DwarfRegNum<[13]>;
def SP  : GR<14, "sp">,  DwarfRegNum<[14]>;
def PC  : GR<15, "pc">,  DwarfRegNum<[15]>;

def PSL : GR<15, "psl">,  DwarfRegNum<[16]>;

def GPRC : RegisterClass<"VAX", [i32], 32,
    (add R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, AP, FP, SP, PC, PSL)>;

def SR :  RegisterClass<"VAX", [i32], 32, (add PSL)>;

def GPRCX :  RegisterClass<"VAX", [i32], 32, (add (sub GPRC, SP, PC, PSL))>;
