cmake_minimum_required(VERSION 3.25)

project(ClapPluginCppTemplate LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/Dependencies.cmake)


add_library(${PROJECT_NAME} MODULE
    src/Utils.h
    src/Plugin.h
    src/Plugin.cpp
    src/Factory.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC clap clap-helpers)

if (APPLE)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()


target_precompile_headers(${PROJECT_NAME} PRIVATE
        <clap/helpers/plugin.hh>
        <clap/helpers/host-proxy.hxx>
        <clap/helpers/plugin.hxx>
        <iostream>
)

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
