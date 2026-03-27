# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/TetrisGame_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/TetrisGame_autogen.dir/ParseCache.txt"
  "TetrisGame_autogen"
  )
endif()
