cmake_minimum_required(VERSION 3.16)

# Application Version
set(APP_VERSION_MAJOR 0)
set(APP_VERSION_MINOR 2)
set(APP_VERSION_PATCH 0)
set(APP_NAME "World")
set(target_name "world")

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# -------------------------------
# Toolchain
# -------------------------------
set(ZOS_TOOLCHAIN sdcc)
include($ENV{ZOS_PATH}/cmake/zos_init.cmake)

# -------------------------------
# Project
# -------------------------------
project(${target_name} C ASM)

# -------------------------------
# Executable
# -------------------------------
add_executable(${target_name}
    src/main.c
    src/assets.c
    src/player.c
    src/tile.c
    src/engine/ui_map.c
    src/engine/world.c
)

# # -------------------------------
# # Include and link SDKs
# # -------------------------------
zos_use(zvb)
find_package(ZVB_SDK REQUIRED)
target_link_libraries(${target_name} PUBLIC zvb_gfx zvb_sound)

zos_use(coreutils)
find_package(COREUTILS REQUIRED)
target_link_libraries(${target_name} PUBLIC core)

zos_use(zgdk)
find_package(ZGDK REQUIRED)
target_link_libraries(${target_name} PUBLIC zgdk)

zos_use(zar)
find_package(ZAR REQUIRED)
target_link_libraries(${target_name} PUBLIC zar)

target_compile_options(${target_name} PRIVATE
    $<$<COMPILE_LANGUAGE:C>:--nostdlib>
)


# -------------------------------
# Assets
# -------------------------------

gif2zeal(${target_name} FILES
    "assets/spritesheet.gif"
    COMPRESSED
    STRIP 128)

tiled2zeal(${target_name} FILES
    "assets/world.tmx"
    VERBOSE
    LAYER 1
    SIZE 20x15
    OUTPUT "assets/__scrn.map"
    COMPRESSED)

zar_create(${target_name}
    INPUT assets
    OUTPUT ${target_name}.zar)

add_dependencies(${target_name}_world_zar
    ${target_name}_spritesheet_asset
    ${target_name}_world_tiled_asset)


# -------------------------------
# Convert to binary
# -------------------------------
zos_add_outputs(${target_name})
