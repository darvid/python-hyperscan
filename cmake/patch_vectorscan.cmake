if(NOT DEFINED VECTORSCAN_SOURCE_DIR)
  message(FATAL_ERROR "VECTORSCAN_SOURCE_DIR is required")
endif()

set(_parser_file "${VECTORSCAN_SOURCE_DIR}/src/parser/Parser.rl")
if(EXISTS "${_parser_file}")
  file(READ "${_parser_file}" _parser_content)
  string(REPLACE
    "alphtype unsigned char;\n"
    "alphtype unsigned char;\n    getkey ((unsigned char)*p);\n"
    _parser_content
    "${_parser_content}")
  file(WRITE "${_parser_file}" "${_parser_content}")
else()
  message(WARNING "Vectorscan parser file not found at ${_parser_file}")
endif()

if(DEFINED VECTORSCAN_PATCH_X86_64_V2 AND VECTORSCAN_PATCH_X86_64_V2)
  foreach(_file IN ITEMS
      "${VECTORSCAN_SOURCE_DIR}/cmake/cflags-x86.cmake"
      "${VECTORSCAN_SOURCE_DIR}/cmake/archdetect.cmake")
    if(EXISTS "${_file}")
      file(READ "${_file}" _content)
      string(REPLACE "x86-64-v2" "nehalem" _content "${_content}")
      file(WRITE "${_file}" "${_content}")
    else()
      message(WARNING "Vectorscan patch target not found at ${_file}")
    endif()
  endforeach()
endif()
