//===-- VAXInstrFormats.td - VAX Instruction Formats ---*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//
class InstVAX<dag outs, dag ins, string asmstr, list<dag> pattern>
    : Instruction {

    let Namespace = "VAX";
    dag OutOperandList = outs;
    dag InOperandList = ins;
    let AsmString   = asmstr;
    let Pattern = pattern;

    dag Inst = (ascend);

  // Number of bytes
    let Size = 0;
}

class VAXEncOpRegister<string reg_opnd> {
    dag Value = (ascend (operand "$"#reg_opnd, 4),  0b0101);
}

#ifdef NOTYET        
// instruction with 3 registers and small opcode
class InstVAXOSR3<bits<8> op, dag outs, dag ins, string asmstr, list<dag> pattern> : InstVAX<outs, ins, asmstr, pattern> {

      let Inst{7-0} = op;
      // 11-8 rx
      let Inst{15-12} = 5;
      // 19-16 rx
      let Inst{23-20} = 5;
      // 27-24
      let Inst{31-28} = 5;
}

// instruction with 3 registers and large opcode
class InstVAXOBR3<bits<16> op, dag outs, dag ins, string asmstr, list<dag> pattern> : InstVAX<outs, ins, asmstr, pattern>
      let Inst{15-0} = op;
      // 19-16 rx
      let Inst{23-20} = 5;
      // 27-24 rx
      let Inst{31-28} = 5;
      // 35-32 rx
      let Inst{39-36} = 5;
}
#endif
