cmake_minimum_required(VERSION 2.8.4)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(PROJECT_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(3DPARTY_LIB_DIR "")		# record where to find the 3rdParty libraries
set(SYSTEM_LIBS "")
set(COMPILE_OPTIONS "")


#
# project settings
#
project(CodeCorner)

if (DEBUG)
	add_definitions(-DDEBUG)
	
	if (MSVC)
	
	else()
		set(COMPILE_OPTIONS "-ggdb -O0")
	endif()
endif()

if (MSVC)
	#
	# define preprocessors
	#
	add_definitions(/wd"4244" /wd"4267" /wd"4996" /wd"4800")

    add_definitions(-DWIN32)
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	add_definitions(-DHILUO)
	if (MSVC12)
		set(3DPARTY_LIB_DIR "vs2013/"${CMAKE_BUILD_TYPE})
	endif()
	
else()
    add_definitions(-DLINUX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_OPTIONS} -std=c++11")
	set(3DPARTY_LIB_DIR "linux")
	set(SYSTEM_LIBS dl pthread)
endif()

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_ROOT_PATH}/bin")

include_directories("${PROJECT_ROOT_PATH}/3rdParty")
include_directories("${PROJECT_ROOT_PATH}/3rdParty/boost_1_60_0")
include_directories("${PROJECT_ROOT_PATH}/3rdParty/lua-5.2.3")
include_directories("${PROJECT_ROOT_PATH}/3rdParty/gtest-1.7.0/include")
include_directories("${PROJECT_ROOT_PATH}/3rdParty/log4cplus-1.1.2/include")
include_directories("${PROJECT_ROOT_PATH}/3rdParty/tbb42/include")
include_directories("${PROJECT_ROOT_PATH}/test/incl")
include_directories("${PROJECT_ROOT_PATH}/src/common/src")

link_directories("${PROJECT_ROOT_PATH}/lib/${3DPARTY_LIB_DIR}")
link_directories("${PROJECT_ROOT_PATH}/3rdParty/boost_1_60_0/stage64/lib")

#
# include the subdirectories
#
add_subdirectory(src/common)
add_subdirectory(src/net)
add_subdirectory(src/algorithm)

#
# include the applications
#
add_subdirectory(App/webspy)