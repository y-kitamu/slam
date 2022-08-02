set(module_name "slam")

CREATE_LIB_FROM_DIR("${module_name}_extension" ${CMAKE_CURRENT_SOURCE_DIR}/extension)

set(LIBRARIES
  ${LIBRARIES}
  ${module_name}_extension
  CACHE INTERNAL ""
)
