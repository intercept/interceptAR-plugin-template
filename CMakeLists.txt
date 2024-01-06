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
    target_compile_options(
        ${INTERCEPT_PLUGIN_NAME} PRIVATE 

        #"-fmodules-ts"

        "$<$<CONFIG:Release>:-O3>"
        #"-s" #remove -s to enable debug symbols. Also maybe add -g or -g3
        "-g"
        "-fPIC"
        "-fpermissive"
        "-pthread"
        "-static-libgcc"
        "-static-libstdc++"
        "-Wno-ignored-attributes"
        "-static"
        "$<$<CONFIG:Release>:-ffunction-sections>"
        "$<$<CONFIG:Release>:-fdata-sections>"
    )
    target_link_options(${INTERCEPT_PLUGIN_NAME} PRIVATE "-fPIC"
        "$<$<CONFIG:Release>:-Wl,--gc-sections>"
        "$<$<OR:$<CONFIG:Release>>:-Wl,--strip-all>"
        "-Wl,-rpath,$ORIGIN"
    ) 
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

    target_link_libraries(${INTERCEPT_PLUGIN_NAME} "libstdc++.a")
    find_package (Threads)
    target_link_libraries (${INTERCEPT_PLUGIN_NAME} ${CMAKE_THREAD_LIBS_INIT})
    #add_dependencies(${INTERCEPT_PLUGIN_NAME} interceptCommon.gcm)

    #add_custom_target("gcmLink${INTERCEPT_PLUGIN_NAME}" ALL COMMAND ${CMAKE_COMMAND} -E create_symlink "../gcm.cache" "gcm.cache") # #TODO custom for plugin template
    #add_dependencies(${INTERCEPT_PLUGIN_NAME} "gcmLink${INTERCEPT_PLUGIN_NAME}")

    target_compile_definitions(${INTERCEPT_PLUGIN_NAME} PRIVATE
        "INTERCEPT_NO_MODULE"
        "$<$<CONFIG:Debug>:_DEBUG>"
    )

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
