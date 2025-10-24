# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(NRF-Code)

target_sources(app PRIVATE
  src/main.c
  src/Lcd_Display/DisplayManager.c

  # src/motors/motor.c
  # src/common/common.c
)
target_include_directories(app PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Lcd_Display
  # ${CMAKE_CURRENT_SOURCE_DIR}/src/motors
  ${CMAKE_CURRENT_SOURCE_DIR}/src/common
)
# Adding Extra Library
zephyr_library_include_directories(${ZEPHYR_BASE}/samples/bluetooth)
