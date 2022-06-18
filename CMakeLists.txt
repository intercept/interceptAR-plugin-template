cmake_minimum_required (VERSION 3.13)
project (InterceptARExample CXX ASM)


if(MSVC)
    set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /std:c++latest" )
#elseif(MSVC)
#    message(FATAL_ERROR "ERROR: You need a C++17 compatible compiler")
endif()

message("GENERATOR USED: '${CMAKE_GENERATOR}'")
message("COMPILER USED: '${CMAKE_CXX_COMPILER_ID}'")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/release/")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(PREBUILT_MODULE_PATH ${CMAKE_BINARY_DIR}/modules)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Global flags I want in every project
add_definitions(/DNOMINMAX)

# Include intercept common
add_subdirectory(lib/InterceptAR/src/common)


# Plugin's filename
set(INTERCEPT_PLUGIN_NAME "examplePlugin")

file(GLOB PluginSources *.hpp *.cpp *.ixx)
SOURCE_GROUP(${INTERCEPT_PLUGIN_NAME} FILES ${PluginSources})

add_library(${INTERCEPT_PLUGIN_NAME} SHARED ${PluginSources} ${CommonSources})

if(CMAKE_COMPILER_IS_GNUCXX)
    #TODO
else()
    target_compile_options(
        ${INTERCEPT_PLUGIN_NAME} PRIVATE 
        "/MP" 
        
        "$<$<CONFIG:Release>:/Zi>"
        "$<$<CONFIG:Debug>:/ZI>"

        "$<$<CONFIG:Release>:/MT>"
        "$<$<CONFIG:Debug>:/MTd>"
        "$<$<CONFIG:Release>:/Ox>"
        "$<$<CONFIG:Release>:/Ob2>"
        "$<$<CONFIG:Release>:/Oi>"
        "$<$<CONFIG:Release>:/Ot>"
    )
    target_link_options(${INTERCEPT_PLUGIN_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:REF>" "$<$<CONFIG:Release>:/OPT:ICF>" "/ENTRY:InterceptEntryPoint") 
endif()

#Binary signing
if(EXISTS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sign.bat" AND MSVC)
    add_custom_command(TARGET ${INTERCEPT_PLUGIN_NAME}
        POST_BUILD
        COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sign.bat
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
endif()
