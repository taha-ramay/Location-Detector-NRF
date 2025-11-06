# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(NRF-Code)

target_sources(app PRIVATE
  src/main.c
  # src/common/common.c
  src/Lcd_Display/DisplayManager.c
  src/Buzzer/Buzzer_Manager.c
  src/BluetoothManager/BluetoothManager.c
  src/Network_Manager/Network_Manager.c
  src/Network_Manager/Network_Manager.c

)
target_include_directories(app PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Lcd_Display
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Buzzer
  ${CMAKE_CURRENT_SOURCE_DIR}/src/common
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Network_Manager
  ${CMAKE_CURRENT_SOURCE_DIR}/src/BluetoothManager

)
# Adding Extra Library
zephyr_library_include_directories(${ZEPHYR_BASE}/samples/bluetooth)
# zephyr_include_directories(drivers/ethernet)
zephyr_library_include_directories(${ZEPHYR_BASE}/drivers/ethernet)
