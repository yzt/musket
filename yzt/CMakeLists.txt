cmake_minimum_required (VERSION 3.0)	# C# support was addid in 3.8

project ("yztMusket" CXX)
#http://www.cmake.org/Wiki/CMake_Useful_Variables

#option(BUILD_TESTS "Build tests" ON)
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE INTERNAL "" FORCE)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/out)
set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/out)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set (G_DEP_LIB_ROOT "${CMAKE_SOURCE_DIR}/dep/lib/win")
    add_definitions (-DONI_OS_WINDOWS)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (G_DEP_LIB_ROOT "${CMAKE_SOURCE_DIR}/dep/lib/lnx")
    add_definitions (-DONI_OS_LINUX)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (G_DEP_LIB_ROOT "${CMAKE_SOURCE_DIR}/dep/lib/osx")
    add_definitions (-DONI_OS_OSX)
else ()
endif ()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set (G_DEP_LIB_BITNESS "${G_DEP_LIB_ROOT}/x64")
    add_definitions (-DONI_64_BITS)
else ()
    set (G_DEP_LIB_BITNESS "${G_DEP_LIB_ROOT}/x86")
    add_definitions (-DONI_32_BITS)
endif ()

include_directories ("include" "dep/include")
link_directories (${G_DEP_LIB_ROOT} ${G_DEP_LIB_BITNESS})

#add_definitions (-DDEAD_MAGE)

if (MSVC)
    add_definitions (/WX)		# Warnings as errors
    add_definitions (/W4)		# Warning level 4
    add_definitions (/wd4100)	# Disable warning about "unreferenced formal parameter"
    add_definitions (/EHsc)
    add_definitions (-DMAGE_COMPILER_MSC)
    add_definitions (-D_CRT_SECURE_NO_WARNINGS)
    add_definitions (/std:c++latest)
elseif (CMAKE_COMPILER_IS_GNUCXX)
    #add_definitions (-std=c++14)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
    add_definitions (-Wall)
    #add_definitions (-Wno-invalid-offsetof)
    add_definitions (-DMAGE_COMPILER_GCC)
else ()	# hopefully, it's Clang!
    #add_definitions (-std=c++14)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
    add_definitions (-Wall)
    #add_definitions (-Wno-invalid-offsetof)
    add_definitions (-DMAGE_COMPILER_OTHER)
endif ()

#-----------------------------------------------------------------------
#-----------------------------------------------------------------------
#-----------------------------------------------------------------------

add_executable ("yzt_breakout"    #WIN32
    "code/bo_main.cpp"
)
target_link_libraries ("yzt_breakout"
    debug
        "SDL2-staticd"
    debug
        "SDL2maind"
    optimized
        "SDL2-static"
    optimized
        "SDL2main"
)

if (WIN32)
    target_link_libraries ("yzt_breakout"
        general
            "winmm"
            "version"
            "imm32"
    )
else ()
#	target_link_libraries ("yzt_breakout"
#		general
#			"pthread"
#	)
endif ()

#-----------------------------------------------------------------------

#if (BUILD_TESTS)
#    add_executable ("oni_unittests"
#        "code/oni_tests_main.cpp"
#        "code/oni_tests_basics_and_types.cpp"
#        "code/oni_tests_socket_udp.cpp"
#        "code/oni_tests_net_broker.cpp"
#        "code/oni_tests_orderstatistics.cpp"
#    )
#endif (BUILD_TESTS)

#-----------------------------------------------------------------------
#-----------------------------------------------------------------------

#if (MSVC)
#    enable_language (CSharp)
#
#    add_library ("oni_csharp" SHARED
#        "code/oni_game_net.cs"
#    )
#
#    set_target_properties ("oni_csharp" PROPERTIES VS_DOTNET_REFERENCES "System;System.Net")
#endif (MSVC)

#-----------------------------------------------------------------------

#add_library ("ice_sdk" SHARED
#	"include/ice_sdk.h"
#	
#	"code/ice_sdk_main.cpp"
#	"code/ice_sdk_comm.hpp"
#	"code/ice_sdk_comm.cpp"
#)
#
##target_link_libraries ("ice" "ice_util")
#
#target_compile_definitions("ice_sdk" PRIVATE ONI_SHARED)
#target_compile_definitions("ice_sdk" PRIVATE ONI_BUILDING_DLL)
#
#if (MSVC)
#	target_compile_options("ice_sdk" PRIVATE "$<$<CONFIG:Debug>:/MTd>" "$<$<CONFIG:Release>:/MT>")
#endif (MSVC)
#
#if (WIN32)
#	target_link_libraries ("ice_sdk"
#		general
#			"ws2_32"
#	)
#endif (WIN32)
#
#-----------------------------------------------------------------------
#-----------------------------------------------------------------------
#-----------------------------------------------------------------------
