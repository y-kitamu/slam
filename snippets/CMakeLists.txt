# subdirectoryの.cppファイルをexecutableとして追加する
FILE(GLOB dir_list "${CMAKE_CURRENT_SOURCE_DIR}/*")

foreach (directory IN LISTS dir_list)
  if (IS_DIRECTORY ${directory})
    get_filename_component(module_name ${directory} NAME_WE)
    CREATE_SNIPPETS_FROM_DIR(${module_name} ${directory})
  endif()
endforeach()
