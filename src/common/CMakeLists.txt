#
# common is a static library contains the most basic functions
#
project(common)

#
# include the source files
#
set(COMMON_SRC 		src/common.cpp 
					src/string_util.cpp
					src/lua_.cpp
					src/time_.cpp
					src/log_.cpp
					src/file_log.cpp
					src/file_util.cpp
					src/file_.cpp
					src/aio_.cpp
					src/thread_.cpp
					src/system_.cpp
					src/platform_win32.cpp
					src/platform_linux.cpp
					src/sqlite3.c
					src/sqlite_.cpp)
					
set(COMMON_HEADER 	src/platform.h
					src/def.h
					src/common.h
					src/log_.h
					src/file_log.h
					src/string_util.h
					src/lua_.h
					src/time_.h
					src/buffer_.h
					src/file_util.h
					src/file_.h
					src/aio_.h
					src/thread_.h
					src/system_.h
					src/sqlite_.h
					src/locker_.h
					src/singleton.h)

add_library(common STATIC ${COMMON_SRC} ${COMMON_HEADER})
target_link_libraries(common lua)

if (MSVC)
	target_link_libraries(common Ws2_32.lib)
endif()

set_target_properties (common PROPERTIES FOLDER lib)

#
# this project is to test common library
#
project(common_test)

set(COMMON_TEST_SRC	test/string_test.cpp
					test/lua_test.cpp
					test/log_test.cpp
					test/file_test.cpp
					test/main.cpp
					${PROJECT_ROOT_PATH}/test/incl/global.h)

add_executable(common_test ${COMMON_TEST_SRC})
add_dependencies(common_test common)
target_link_libraries(common_test common gtest popt log4cplus.lib)

set_target_properties (common_test PROPERTIES FOLDER test)