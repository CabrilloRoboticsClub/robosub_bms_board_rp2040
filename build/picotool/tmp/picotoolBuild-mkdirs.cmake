# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/isaacw/robosub_bms_board_rp2040/build/_deps/picotool-src"
  "/home/isaacw/robosub_bms_board_rp2040/build/_deps/picotool-build"
  "/home/isaacw/robosub_bms_board_rp2040/build/_deps"
  "/home/isaacw/robosub_bms_board_rp2040/build/picotool/tmp"
  "/home/isaacw/robosub_bms_board_rp2040/build/picotool/src/picotoolBuild-stamp"
  "/home/isaacw/robosub_bms_board_rp2040/build/picotool/src"
  "/home/isaacw/robosub_bms_board_rp2040/build/picotool/src/picotoolBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/isaacw/robosub_bms_board_rp2040/build/picotool/src/picotoolBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/isaacw/robosub_bms_board_rp2040/build/picotool/src/picotoolBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
