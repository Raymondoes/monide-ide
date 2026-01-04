cmake_minimum_required(VERSION 3.10...3.28)

message(STATUS "You need to run from a msys64 shell on windows")

project(Monide_Backend LANGUAGES C CXX)

# Only enforce when Monide_Backend is built standalone (not as subdirectory)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
        message(FATAL_ERROR
            "In-source builds are not allowed. Please create a separate build directory:\n"
            "  mkdir build && cd build && cmake .."
        )
    endif()
endif()

find_package(OpenGL REQUIRED)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Detect platform-specific library directory
if(WIN32)
    set(PLATFORM_LIB_SUFFIX "Windows")
elseif(APPLE)
    set(PLATFORM_LIB_SUFFIX "MacOSX")
elseif(UNIX)
    set(PLATFORM_LIB_SUFFIX "Linux")
endif()

# Third-Party Paths
set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/External")
set(THIRD_PARTY_PLATFORM_DIR "${THIRD_PARTY_DIR}/lib${PLATFORM_LIB_SUFFIX}")
set(THIRD_PARTY_INCLUDES "${THIRD_PARTY_DIR}/3rdParty/include" "${CMAKE_CURRENT_SOURCE_DIR}")
set(BASE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")

include_directories(${THIRD_PARTY_INCLUDES})

# Engine Sources - Use absolute paths and be more specific
file(GLOB_RECURSE Source_CPP
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
file(GLOB_RECURSE Source_C
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)
file(GLOB_RECURSE Source_Headers
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

set(Source ${Source_CPP} ${Source_C})

# Verify we have source files
if(NOT Source)
    message(FATAL_ERROR "No source files found in ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

message(STATUS "Found ${CMAKE_CURRENT_SOURCE_DIR} source files: ${Source}")


add_library(glad STATIC OpenGL/Loader/Glad.c)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Create the library
add_library(Monide_Backend SHARED ${Source} ${Source_Headers})

set_target_properties(Monide_Backend PROPERTIES
    OUTPUT_NAME "Monide_Backend"
    ARCHIVE_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}"
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

# Compiler-specific options
if(MSVC)
    target_compile_options(Monide_Backend PRIVATE /utf-8 /W3)
    # MSVC runtime library flags
    foreach(flag_var CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
                     CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
                     CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                     CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MT")
            string(REGEX REPLACE "/MT" "/MD" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Monide_Backend PRIVATE -Wall -Wextra -Wno-pedantic)
endif()

target_compile_features(Monide_Backend PUBLIC cxx_std_20)

# Platform-specific definitions
target_compile_definitions(Monide_Backend PUBLIC
    Monide_Backend_EXPORTS
    GLM_FORCE_RADIANS
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_ENABLE_EXPERIMENTAL
)

if(WIN32)
    target_compile_definitions(Monide_Backend PUBLIC 
        _PLATFORM_WINDOWS
        VK_USE_PLATFORM_WIN32_KHR
        NOMINMAX
        WIN32_LEAN_AND_MEAN
        _CRT_SECURE_NO_WARNINGS
    )
    target_link_libraries(Monide_Backend PUBLIC
        kernel32 user32 gdi32 winspool shell32 ole32 oleaut32
        uuid comdlg32 advapi32
    )
elseif(APPLE)
    target_compile_definitions(Monide_Backend PUBLIC 
        _PLATFORM_MACOS
        VK_USE_PLATFORM_MACOS_MVK
    )
elseif(UNIX)
    target_compile_definitions(Monide_Backend PUBLIC 
        _PLATFORM_LINUX
        VK_USE_PLATFORM_XLIB_KHR
    )
    target_link_libraries(Monide_Backend PUBLIC X11 pthread dl)
endif()

# Link OpenGL
target_link_libraries(Monide_Backend PUBLIC OpenGL::GL)

# Find and link GLFW
if(WIN32)
    # On Windows, use the bundled GLFW library
    set(GLFW_LIB_PATH "${THIRD_PARTY_PLATFORM_DIR}/libglfw3.a")
    if(EXISTS "${GLFW_LIB_PATH}")
        target_link_libraries(Monide_Backend PUBLIC "${GLFW_LIB_PATH}")
        message(STATUS "Linking GLFW from: ${GLFW_LIB_PATH}")
    else()
        message(FATAL_ERROR "GLFW library not found at: ${GLFW_LIB_PATH}")
    endif()
elseif(UNIX)
    # On Linux/macOS, use find_package to locate system GLFW
    find_package(glfw3 REQUIRED)
    target_link_libraries(Monide_Backend PUBLIC glfw)
    message(STATUS "Found and linked system GLFW")
endif()

# Copy DLLs to build directory after build
if(WIN32)
    # Copy glfw3.dll if it exists
    set(GLFW_DLL_PATH "${THIRD_PARTY_PLATFORM_DIR}/glfw3.dll")
    if(EXISTS "${GLFW_DLL_PATH}")
        add_custom_command(TARGET Monide_Backend POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GLFW_DLL_PATH}"
            "${BASE_OUTPUT_DIR}"
            COMMENT "Copying glfw3.dll to build directory"
        )
    endif()
endif()