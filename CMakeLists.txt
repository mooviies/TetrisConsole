cmake_minimum_required(VERSION 3.16)
project(TetrisConsole LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Media embedding ---
file(GLOB MEDIA_FILES TetrisConsole/media/*)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/media_data.h ${CMAKE_BINARY_DIR}/media_data.cpp
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}
    COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/embed_media.py ${CMAKE_BINARY_DIR}
    DEPENDS ${MEDIA_FILES} ${CMAKE_SOURCE_DIR}/scripts/embed_media.py
    COMMENT "Embedding media files"
)

# --- Gather sources ---
file(GLOB KONSOLE_SRCS TetrisConsole/source/Konsole/*.cpp)
file(GLOB TETRIS_SRCS  TetrisConsole/source/Tetris/*.cpp)

if(WIN32)
    list(FILTER KONSOLE_SRCS EXCLUDE REGEX "Linux\\.cpp$")
else()
    list(FILTER KONSOLE_SRCS EXCLUDE REGEX "Win32\\.cpp$")
endif()

# --- Executable ---
if(WIN32)
    set(TARGET_NAME TetrisConsole)
else()
    set(TARGET_NAME tetris)
endif()

add_executable(${TARGET_NAME}
    ${KONSOLE_SRCS}
    ${TETRIS_SRCS}
    ${CMAKE_BINARY_DIR}/media_data.cpp
)

# --- Include paths ---
target_include_directories(${TARGET_NAME} PRIVATE
    TetrisConsole/source/Tetris
    TetrisConsole/source/Konsole
)
target_include_directories(${TARGET_NAME} SYSTEM PRIVATE
    TetrisConsole/include
    ${CMAKE_BINARY_DIR}
)

# --- Compiler flags ---
if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W3 /sdl)
else()
    target_compile_options(${TARGET_NAME} PRIVATE
        -Wall -Wextra -Wpedantic -Wshadow -Wconversion
    )
endif()

# --- Platform linking ---
if(WIN32)
    target_link_libraries(${TARGET_NAME} PRIVATE winmm ole32)
elseif(APPLE)
    find_library(COREAUDIO_LIB CoreAudio REQUIRED)
    find_library(AUDIOTOOLBOX_LIB AudioToolbox REQUIRED)
    find_library(COREFOUNDATION_LIB CoreFoundation REQUIRED)
    target_link_libraries(${TARGET_NAME} PRIVATE
        pthread dl m
        ${COREAUDIO_LIB} ${AUDIOTOOLBOX_LIB} ${COREFOUNDATION_LIB}
    )
else()
    target_link_libraries(${TARGET_NAME} PRIVATE pthread dl m)
endif()
