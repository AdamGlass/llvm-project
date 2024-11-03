add_llvm_component_group(VAX)
set(LLVM_TARGET_DEFINITIONS VAX.td)

add_llvm_target(VAXCodeGen
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
