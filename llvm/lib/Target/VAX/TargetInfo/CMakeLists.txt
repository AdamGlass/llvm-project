add_llvm_component_library(LLVMVAXInfo
  VAXTargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  VAX
  )
