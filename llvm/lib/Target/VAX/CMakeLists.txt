add_llvm_component_group(VAX)
set(LLVM_TARGET_DEFINITIONS VAX.td)

tablegen(LLVM VAXGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM VAXGenRegisterInfo.inc -gen-register-info)

add_public_tablegen_target(VaxCommonTableGen)

add_llvm_target(VAXCodeGen
  VAXRegisterInfo.cpp

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

add_subdirectory(TargetInfo)
