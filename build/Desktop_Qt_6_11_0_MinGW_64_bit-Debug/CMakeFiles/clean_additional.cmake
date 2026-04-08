# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Qthealthmonitor_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Qthealthmonitor_autogen.dir\\ParseCache.txt"
  "Qthealthmonitor_autogen"
  )
endif()
