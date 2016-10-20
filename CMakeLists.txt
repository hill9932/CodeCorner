cmake_minimum_required(VERSION 2.8.4)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(PROJECT_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(3DPARTY_LIB_DIR "")		# record where to find the 3rdParty libraries

#
# define preprocessors
#
add_definitions(/wd"4244" /wd"4267" /wd"4996" /wd"4800")


#
# project settings
#
project(CodeCorner)

if (UNIX)
    add_definitions(-DLINUX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	set(3DPARTY_LIB_DIR "linux")
	
elseif (MSVC)
    add_definitions(-DWIN32)
	add_definitions(-DWIN32_LEAN_AND_MEAN)

	if (MSVC12)
		set(3DPARTY_LIB_DIR "vs2013/"${CMAKE_BUILD_TYPE})
	endif()
endif()

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_ROOT_PATH}/bin")

include_directories("${PROJECT_ROOT_PATH}/3rdParty")
include_directories("${PROJECT_ROOT_PATH}/3rdParty/boost_1_60_0")
include_directories("${PROJECT_ROOT_PATH}/3rdParty/lua-5.2.3")
include_directories("${PROJECT_ROOT_PATH}/3rdParty/gtest-1.7.0/include")
include_directories("${PROJECT_ROOT_PATH}/3rdParty/log4cplus-1.1.2/include")
include_directories("${PROJECT_ROOT_PATH}/3rdParty/tbb42/include")
include_directories("${PROJECT_ROOT_PATH}/test/incl")

link_directories("${PROJECT_ROOT_PATH}/lib/${3DPARTY_LIB_DIR}")
link_directories("${PROJECT_ROOT_PATH}/3rdParty/boost_1_60_0/stage64/lib")

#
# include the subdirectories
#
add_subdirectory(src/common)
add_subdirectory(src/net)

#
# include the applications
#
add_subdirectory(app/webspy)