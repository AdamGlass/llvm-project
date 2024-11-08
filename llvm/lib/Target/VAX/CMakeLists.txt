add_llvm_component_group(VAX)
set(LLVM_TARGET_DEFINITIONS VAX.td)

tablegen(LLVM VAXGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM VAXGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM VAXGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM VAXGenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(VAXCommonTableGen)

add_llvm_target(VAXCodeGen
  VAXFrameLowering.cpp
  VAXInstrInfo.cpp
  VAXRegisterInfo.cpp
  VAXSubtarget.cpp

  LINK_COMPONENTS
  Analysis
  AsmPrinter
  CodeGen
  CodeGenTypes
  Core
  MC
  SelectionDAG
  Support
  Target
  TargetParser
  TransformUtils
  VAXDesc
  VAXInfo

  ADD_TO_COMPONENT
  VAX
  )

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
