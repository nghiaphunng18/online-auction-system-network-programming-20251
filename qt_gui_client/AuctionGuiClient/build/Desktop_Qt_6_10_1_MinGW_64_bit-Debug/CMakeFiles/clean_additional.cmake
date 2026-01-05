# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "AuctionGuiClient_autogen"
  "CMakeFiles\\AuctionGuiClient_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\AuctionGuiClient_autogen.dir\\ParseCache.txt"
  )
endif()
