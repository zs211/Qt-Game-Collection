# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/PlaneGame_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/PlaneGame_autogen.dir/ParseCache.txt"
  "PlaneGame_autogen"
  )
endif()
