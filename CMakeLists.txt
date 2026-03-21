cmake_minimum_required(VERSION 3.6)
project(myDemo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif()

set(PROJECT_DIR ${CMAKE_SOURCE_DIR})

include_directories(${PROJECT_DIR}/include)
include_directories(${PROJECT_DIR}/src/inc)

link_directories(${PROJECT_DIR}/thirdParty)

if(WIN32)
    set(LIBS glfw3 opengl32 assimp freetype)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(UNIX AND NOT APPLE)
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
    set(LIBS glfw3 X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread freetype assimp)
elseif(APPLE)
    find_package(Freetype REQUIRED)
    INCLUDE_DIRECTORIES(/System/Library/Frameworks)
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
    FIND_LIBRARY(IOKit_LIBRARY IOKit)
    FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
    SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
    SET(APPLE_LIBS ${APPLE_LIBS} glfw3 assimp ${FREETYPE_LIBRARIES})
    set(LIBS ${LIBS} ${APPLE_LIBS})
endif()

add_library(STB_IMAGE ${PROJECT_DIR}/src/stb_image.cpp)
set(LIBS ${LIBS} STB_IMAGE)

add_library(GLAD ${PROJECT_DIR}/src/glad.c)
set(LIBS ${LIBS} GLAD)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_CONFIGURATION_TYPES)
    set(LIBS ${LIBS} debug shaderc_combinedd optimized shaderc_combined)
else()
    set(LIBS ${LIBS} shaderc_combined)
endif()

file(GLOB SOURCES 
    "${PROJECT_DIR}/src/*.cpp"
    "${PROJECT_DIR}/src/rendergraph/*.cpp"
    "${PROJECT_DIR}/src/deferred/*.cpp"
)

file(GLOB HEADERS
    "${PROJECT_DIR}/src/inc/*.h"
    "${PROJECT_DIR}/src/inc/rendergraph/*.h"
    "${PROJECT_DIR}/src/inc/deferred/*.h"
)

add_executable(myDemo ${SOURCES} ${HEADERS})
target_link_libraries(myDemo ${LIBS})

if(MSVC)
    target_compile_options(myDemo PRIVATE /std:c++17 /MP)
    target_link_options(myDemo PUBLIC /ignore:4099)
endif()

if(WIN32)
    set_target_properties(myDemo PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_DIR}/bin")
    set_target_properties(myDemo PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}/bin")
elseif(UNIX AND NOT APPLE)
    set_target_properties(myDemo PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_DIR}/bin")
elseif(APPLE)
    set_target_properties(myDemo PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_DIR}/bin")
endif()

file(GLOB SHADERS 
    "${PROJECT_DIR}/shader/*.vs"
    "${PROJECT_DIR}/shader/*.fs"
    "${PROJECT_DIR}/shader/*.gs"
    "${PROJECT_DIR}/shader/*.tcs"
    "${PROJECT_DIR}/shader/*.tes"
    "${PROJECT_DIR}/shader/*.cs"
    "${PROJECT_DIR}/shader/*.glsl"
    "${PROJECT_DIR}/shader/deferred/*.vs"
    "${PROJECT_DIR}/shader/deferred/*.fs"
)

file(GLOB SHADER_COMMON 
    "${PROJECT_DIR}/shader/common/*.glsl"
)

file(GLOB DLLS "${PROJECT_DIR}/thirdParty/dlls/*.dll")

foreach(SHADER ${SHADERS})
    if(WIN32)
        get_filename_component(SHADER_DIR ${SHADER} DIRECTORY)
        if(SHADER_DIR MATCHES "deferred")
            add_custom_command(TARGET myDemo PRE_BUILD 
                COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:myDemo>/shader/deferred
                COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:myDemo>/shader/deferred/)
        else()
            add_custom_command(TARGET myDemo PRE_BUILD 
                COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:myDemo>/shader
                COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:myDemo>/shader/)
        endif()
    elseif(UNIX AND NOT APPLE)
        get_filename_component(SHADER_DIR ${SHADER} DIRECTORY)
        if(SHADER_DIR MATCHES "deferred")
            file(COPY ${SHADER} DESTINATION ${PROJECT_DIR}/bin/shader/deferred/)
        else()
            file(COPY ${SHADER} DESTINATION ${PROJECT_DIR}/bin/shader/)
        endif()
    elseif(APPLE)
        get_filename_component(SHADERNAME ${SHADER} NAME)
        get_filename_component(SHADER_DIR ${SHADER} DIRECTORY)
        if(SHADER_DIR MATCHES "deferred")
            add_custom_command(TARGET myDemo POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${SHADER} ${PROJECT_DIR}/bin/shader/deferred/${SHADERNAME})
        else()
            add_custom_command(TARGET myDemo POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${SHADER} ${PROJECT_DIR}/bin/shader/${SHADERNAME})
        endif()
    endif()
endforeach()

foreach(SHADER ${SHADER_COMMON})
    if(WIN32)
        add_custom_command(TARGET myDemo PRE_BUILD 
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:myDemo>/shader/common
            COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:myDemo>/shader/common/)
    elseif(UNIX AND NOT APPLE)
        file(COPY ${SHADER} DESTINATION ${PROJECT_DIR}/bin/shader/common/)
    elseif(APPLE)
        get_filename_component(SHADERNAME ${SHADER} NAME)
        add_custom_command(TARGET myDemo POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${SHADER} ${PROJECT_DIR}/bin/shader/common/${SHADERNAME})
    endif()
endforeach()

if(WIN32 AND DLLS)
    add_custom_command(TARGET myDemo PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLLS} $<TARGET_FILE_DIR:myDemo>)
endif()
