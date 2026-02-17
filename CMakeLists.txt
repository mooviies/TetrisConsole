cmake_minimum_required(VERSION 3.16)
project(Tetrominos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Compiler flags (shared) ---
if(MSVC)
    add_compile_options(/W3 /sdl)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion)
endif()

# --- Konsole static library ---
file(GLOB_RECURSE KONSOLE_SRCS TetrisConsole/source/Konsole/*.cpp)

if(WIN32)
    list(FILTER KONSOLE_SRCS EXCLUDE REGEX "Linux\\.cpp$")
else()
    list(FILTER KONSOLE_SRCS EXCLUDE REGEX "Win32\\.cpp$")
endif()

add_library(konsole STATIC ${KONSOLE_SRCS})

target_include_directories(konsole PUBLIC
    TetrisConsole/source/Konsole/Platform
    TetrisConsole/source/Konsole/UI
    TetrisConsole/source/Konsole/Util
)
target_include_directories(konsole SYSTEM PUBLIC
    TetrisConsole/include
    ${CMAKE_BINARY_DIR}
)

if(WIN32)
    target_link_libraries(konsole PUBLIC winmm ole32)
elseif(APPLE)
    find_library(COREAUDIO_LIB CoreAudio REQUIRED)
    find_library(AUDIOTOOLBOX_LIB AudioToolbox REQUIRED)
    find_library(COREFOUNDATION_LIB CoreFoundation REQUIRED)
    target_link_libraries(konsole PUBLIC
        pthread dl m
        ${COREAUDIO_LIB} ${AUDIOTOOLBOX_LIB} ${COREFOUNDATION_LIB}
    )
else()
    target_link_libraries(konsole PUBLIC pthread dl m)
endif()

# --- Media embedding ---
file(GLOB MEDIA_FILES TetrisConsole/media/*)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/media_data.h ${CMAKE_BINARY_DIR}/media_data.cpp
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}
    COMMAND python ${CMAKE_SOURCE_DIR}/scripts/embed_media.py ${CMAKE_BINARY_DIR}
    DEPENDS ${MEDIA_FILES} ${CMAKE_SOURCE_DIR}/scripts/embed_media.py
    COMMENT "Embedding media files"
)
add_custom_target(media_embed DEPENDS ${CMAKE_BINARY_DIR}/media_data.h)
add_dependencies(konsole media_embed)

# --- Game executable ---
file(GLOB_RECURSE GAME_SRCS TetrisConsole/source/Game/*.cpp)

if(WIN32)
    set(TARGET_NAME Tetrominos)
else()
    set(TARGET_NAME tetrominos)
endif()

add_executable(${TARGET_NAME}
    ${GAME_SRCS}
    ${CMAKE_BINARY_DIR}/media_data.cpp
        TetrisConsole/source/Game/Rules/GoalPolicy.cpp
        TetrisConsole/source/Game/Rules/GoalPolicy.h
        TetrisConsole/source/Game/Rules/VariantRule.cpp
        TetrisConsole/source/Game/Rules/VariantRule.h
)

target_include_directories(${TARGET_NAME} PRIVATE
    TetrisConsole/source/Game/Core
    TetrisConsole/source/Game/Piece
    TetrisConsole/source/Game/Rules
    TetrisConsole/source/Game/Display
    TetrisConsole/source/Game/Test
)

target_compile_definitions(${TARGET_NAME} PRIVATE
    $<$<CONFIG:Debug>:GAME_DEBUG>
)

target_link_libraries(${TARGET_NAME} PRIVATE konsole)

if(MINGW)
    target_link_options(${TARGET_NAME} PRIVATE -static-libgcc -static-libstdc++ -static -lpthread)
endif()
