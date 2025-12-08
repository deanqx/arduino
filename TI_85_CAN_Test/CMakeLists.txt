cmake_minimum_required(VERSION 4.1)
project(TI_x C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(MCU atmega2560)
set(F_CPU 16000000UL)          
set(PROGRAMMER wiring)
set(PORT /dev/ttyUSB0 CACHE STRING "Port of programmer")

# Generic: no OS
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER avr-gcc)

# Compiler flags
add_definitions(-DF_CPU=${F_CPU})
set(CMAKE_C_FLAGS "-mmcu=${MCU} -Os")
set(CMAKE_EXE_LINKER_FLAGS "-mmcu=${MCU}")

add_executable(${PROJECT_NAME}.elf
    src/main.c
)

add_subdirectory(libs/usart)
add_subdirectory(libs/can)
target_link_libraries(${PROJECT_NAME}.elf
    PRIVATE
    usart
    can
)

# convert ELF to IHEX
add_custom_command(
    OUTPUT ${PROJECT_NAME}.hex
    DEPENDS ${PROJECT_NAME}.elf
    COMMAND avr-objcopy -O ihex -R .eeprom ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
)

add_custom_target(hex
    DEPENDS ${PROJECT_NAME}.hex
)

add_custom_target(upload
    ALL
    DEPENDS hex
    COMMAND avrdude -p atmega328p -P ${PORT} -c arduino -U flash:w:${PROJECT_NAME}.hex
# COMMAND avrdude -p ${MCU} -D -P ${PORT} -c ${PROGRAMMER} -U flash:w:${PROJECT_NAME}.hex
)
