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

# --- KonsoleGE library (submodule) ---
add_subdirectory(KonsoleGE)

# --- Media embedding ---
file(GLOB MEDIA_FILES Tetrominos/media/*)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/media_data.h ${CMAKE_BINARY_DIR}/media_data.cpp
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}
    COMMAND python ${CMAKE_SOURCE_DIR}/KonsoleGE/scripts/embed_media.py
            ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/Tetrominos/media
    DEPENDS ${MEDIA_FILES} ${CMAKE_SOURCE_DIR}/KonsoleGE/scripts/embed_media.py
    COMMENT "Embedding media files"
)
add_custom_target(media_embed DEPENDS ${CMAKE_BINARY_DIR}/media_data.h)
add_dependencies(konsolege media_embed)

# --- Game executable ---
file(GLOB_RECURSE GAME_SRCS Tetrominos/source/*.cpp)

if(WIN32)
    set(TARGET_NAME Tetrominos)
else()
    set(TARGET_NAME tetrominos)
endif()

add_executable(${TARGET_NAME}
    ${GAME_SRCS}
    ${CMAKE_BINARY_DIR}/media_data.cpp
        Tetrominos/source/Rules/GoalPolicy.cpp
        Tetrominos/source/Rules/GoalPolicy.h
        Tetrominos/source/Rules/VariantRule.cpp
        Tetrominos/source/Rules/VariantRule.h
)

target_include_directories(${TARGET_NAME} PRIVATE
    Tetrominos/source/Core
    Tetrominos/source/Piece
    Tetrominos/source/Rules
    Tetrominos/source/Display
    Tetrominos/source/Test
)

target_compile_definitions(${TARGET_NAME} PRIVATE
    $<$<CONFIG:Debug>:GAME_DEBUG>
)

target_link_libraries(${TARGET_NAME} PRIVATE konsolege)

if(MINGW)
    target_link_options(${TARGET_NAME} PRIVATE -static-libgcc -static-libstdc++ -static -lpthread)
endif()
