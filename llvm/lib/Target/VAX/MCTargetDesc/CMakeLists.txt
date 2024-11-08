add_llvm_component_library(LLVMVAXDesc
  VAXInstPrinter.cpp
  VAXMCTargetDesc.cpp
  VAXMCAsmInfo.cpp

  LINK_COMPONENTS
  MC
  Support
  VAXInfo

  ADD_TO_COMPONENT
  VAX
  )
