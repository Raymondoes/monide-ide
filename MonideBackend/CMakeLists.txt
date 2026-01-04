cmake_minimum_required(VERSION 3.7)

message("You need to run from a msys64 shell on windows bruh")
project("Monide IDE"
    VERSION 1.0.0
    DESCRIPTION "Monide IDE"
    LANGUAGES C CXX
)

# Language standards
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Detect architecture for current build folder
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_SUFFIX "x64")
else()
    set(ARCH_SUFFIX "x86")
endif()

# Set output directories in project root
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(BASE_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/000-Build-${CMAKE_BUILD_TYPE}-${ARCH_SUFFIX}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}")
set(CMAKE_PDB_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}")

# Multi-config generators (Visual Studio)
if(CMAKE_CONFIGURATION_TYPES)
    foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${CONFIG} CONFIG_UPPER)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${BASE_OUTPUT_DIR}")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${BASE_OUTPUT_DIR}")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${BASE_OUTPUT_DIR}")
        set(CMAKE_PDB_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${BASE_OUTPUT_DIR}")
    endforeach()
endif()

# Compiler-specific flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    # Suppress specific warnings for GCC/Clang
    add_compile_options(-Wno-pedantic)  # Suppresses the ISO C warnings you're seeing
elseif(MSVC)
    add_compile_options(
        /W3
        /permissive-
        /Zc:__cplusplus
        /EHsc
        /utf-8
        $<$<CONFIG:Debug>:/Zi>
        $<$<CONFIG:Release>:/O2>
        /wd4005 /wd4224 /wd4267 /wd4244  # MSVC-only warning suppressions
    )
    # Linker flags for MSVC only
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099,4098")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /ignore:4099,4098")
endif()

# Export compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Keep build artifacts out of source
add_subdirectory(
    "${CMAKE_SOURCE_DIR}/src"
    "${BASE_OUTPUT_DIR}/Monide_Build"
)