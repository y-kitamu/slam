# CMakeLists.txtが存在するサブディレクトリをすべて走査する
MACRO(ADD_SUBDIRS)
  # 引数を指定すると、そのディレクトリを add_subdirectory から除外する
  FILE(GLOB sub_dirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
  SET(exclude_list "${ARGN}")
  list(LENGTH exclude_list exclude_num)
  FOREACH(dir IN LISTS sub_dirs)
    IF (${exclude_num} GREATER 0)
      IF (dir IN_LIST exclude_list)
        continue()
      ENDIF()
    ENDIF()
    IF (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
      IF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/CMakeLists.txt)
        add_subdirectory(${dir})
      ENDIF()
    ENDIF()
  ENDFOREACH()
ENDMACRO()

# 現在のdirectoryの.cppファイルをexecutableとして追加、作成する
MACRO(ADD_SNIPETS)
  if ("${module_name}" STREQUAL "")
    get_filename_component(module_name ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)
  endif()
  CREATE_SNIPPETS_FROM_DIR(${module_name} ${CMAKE_CURRENT_SOURCE_DIR})
ENDMACRO()

# 指定したdirectory (${directory}) の.cppファイルをexecutable (<basename>_${module_name}) として追加、作成する
MACRO(CREATE_SNIPPETS_FROM_DIR module_name directory)
  message(STATUS "Creating snippets from ${directory}")
  FILE(GLOB file_list "${directory}/[a-z]*.cpp")
  FOREACH(filename IN LISTS file_list)
    get_filename_component(basename ${filename} NAME_WE)
    add_executable(${module_name}_${basename} ${filename})
    if (NOT "${LIBRARIES}" STREQUAL "")
      target_link_libraries(${module_name}_${basename} ${LIBRARIES})
    endif()
    if (NOT "${EXTERNAL_LIBS}" STREQUAL "")
      target_link_libraries(${module_name}_${basename} ${EXTERNAL_LIBS})
    endif()
    if (MSVC)
      set_property(TARGET ${module_name}_${basename} PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    endif()
    message("     Create executable : ${module_name}_${basename}")
  ENDFOREACH()
ENDMACRO()

# 現在のdirectoryからlibraryを作成する
MACRO(ADD_LIB)
  if ("${module_name}" STREQUAL "")
    get_filename_component(module_name ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)
  endif()
  CREATE_LIB_FROM_DIR(${module_name} ${CMAKE_CURRENT_SOURCE_DIR})
ENDMACRO()

# 指定したdirectory (${directory}) のlibrary (library名:${module_name}) を作成する
MACRO(CREATE_LIB_FROM_DIR module_name directory)
  message(STATUS "Create library : ${module_name}")
  FILE(GLOB_RECURSE file_list LIST_DIRECTORIES true "${directory}/[a-z]*.cpp")
  if (file_list)
    if (BUILD_SHARED_LIBS)
      add_library(${module_name} SHARED ${file_list})
      target_link_libraries(${module_name} ${EXTERNAL_LIBS})
    else()
      add_library(${module_name} STATIC ${file_list})
    endif()
    set(LIBRARIES ${LIBRARIES} ${module_name} CACHE INTERNAL "")
  endif()
ENDMACRO()

# テストの作成
MACRO(ADD_GTEST)
  if (NOT "${module_name}")
    get_filename_component(module_name ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)
  endif()

  file(GLOB file_list "${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp")
  foreach (fname IN LISTS file_list)
    get_filename_component(bname ${fname} NAME_WE)
    set(test_name "gtest_${module_name}_${bname}")
    add_executable(${test_name} ${fname})
    target_link_libraries(${test_name} ${LIBRARIES} ${EXTERNAL_LIBS} ${HALIDE_OBJECTS})
    add_test(NAME ${test_name}
      COMMAND "$<TARGET_FILE:${test_name}>")
  endforeach()
ENDMACRO()

# child processの実行 (結果の出力も一緒に行う)
FUNCTION(EXECUTE_PROCESS_WITH_OUTPUT)
  set(oneValueArgs WORKING_DIRECTORY)
  set(multiValueArgs COMMAND)
  cmake_parse_arguments(EXECUTE_PROCESS_WITH_OUTPUT "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  set(command ${EXECUTE_PROCESS_WITH_OUTPUT_COMMAND})
  set(workingdir ${EXECUTE_PROCESS_WITH_OUTPUT_WORKING_DIRECTORY})

  if (NOT EXISTS ${workingdir})
    file(MAKE_DIRECTORY ${workingdir})
  endif()
  message(STATUS "Start command : ${command}, working directory : ${workingdir}")
  execute_process(
    COMMAND ${command}
    WORKING_DIRECTORY ${workingdir}
    RESULT_VARIABLE process_result
    OUTPUT_VARIABLE command_result
    ERROR_VARIABLE command_result
  )
  message(STATUS "Command = ${command},  result = ${process_result}")
  message(STATUS "Command output")
  message("${command_result}")
  message(STATUS "Finish command output")
ENDFUNCTION()
