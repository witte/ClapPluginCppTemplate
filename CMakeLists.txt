cmake_minimum_required(VERSION 4.0.1)

if (APPLE)
    set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang")
    set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")

    # Upstream clang doesn't set macOS's sysroot automatically
    execute_process(
            COMMAND xcrun --show-sdk-path
            OUTPUT_VARIABLE MACOS_SDK_PATH
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_OSX_SYSROOT ${MACOS_SDK_PATH})
elseif (UNIX)
    # TODO: set clang for linux
    #    set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang")
    #    set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
endif ()

project(ClapPluginCppTemplate LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/Dependencies.cmake)


add_library(${PROJECT_NAME} MODULE
    src/Factory.cpp
)

target_sources(${PROJECT_NAME}
    PUBLIC
        FILE_SET modules
        TYPE CXX_MODULES
        FILES
        src/Clap.cppm
        src/Utils.cppm
        src/Plugin.cppm
)

if (APPLE)
    # std.cppm needs to be compiled with:
    # ./clang++ -std=c++23 -stdlib=libc++ -Wno-reserved-identifier -Wno-reserved-module-identifier \
    #     --precompile -o std.pcm /opt/homebrew/opt/llvm/share/libc++/v1/std.cppm
    target_compile_options(${PROJECT_NAME} PRIVATE -fmodule-file=std=/opt/homebrew/Cellar/llvm/19.1.7_1/bin/std.pcm)
elseif (UNIX)
    # TODO: set std.pcm for linux
endif ()

target_link_libraries(${PROJECT_NAME} PUBLIC clap clap-helpers)

if (APPLE)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter)
endif()


#target_precompile_headers(${PROJECT_NAME} PRIVATE
#        <clap/version.h>
#        <clap/clap.h>
#        <clap/plugin-features.h>
#        <clap/helpers/plugin.hh>
#        <clap/helpers/host-proxy.hxx>
#        <clap/helpers/plugin.hxx>
#        <iostream>
#)

# Packaging
if (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUNDLE True
        BUNDLE_EXTENSION clap
        MACOSX_BUNDLE_GUI_IDENTIFIER your.reversed.domain.name.${PROJECT_NAME}
        MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/cmake/ClapPluginCppTemplate.plist.in
    )
endif()
