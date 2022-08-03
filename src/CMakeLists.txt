set(module_name "slam")

CREATE_LIB_FROM_DIR("${module_name}_extension" ${CMAKE_CURRENT_SOURCE_DIR}/extension)
CREATE_LIB_FROM_DIR("${module_name}_debug" ${CMAKE_CURRENT_SOURCE_DIR}/debug)

set(LIBRARIES
  ${LIBRARIES}
  ${module_name}_extension
  ${module_name}_debug
  CACHE INTERNAL ""
)
