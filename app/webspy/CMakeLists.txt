set(PROJECT_NAME	webspy)
project(${PROJECT_NAME})

set(WEBSPY_SRC_FOLD ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(WEBSPY_SRC 		src/main.cpp
					src/basic_manager.cpp
					src/dns_manager.cpp
					src/web_voyager.cpp
					src/web_server.cpp)

set(WEBSPY_HEADER	src/basic_manager.h
					src/dns_manager.h
					src/web_voyager.h
					src/web_server.h)

set(WEBSPY_BASIC	src/config.h
					src/global.h
					src/global.cpp)
					
set(WEBSPY_HTTP_SRC	src/http/basic.h
					src/http/basic.cpp
					src/http/request.h
					src/http/request.cpp)

set(WEBSPY_TASK_SRC	src/task/task.h
					src/task/task.cpp)
					
include_directories("${PROJECT_ROOT_PATH}/3rdParty/libevent-2.1.6-beta")
include_directories("${PROJECT_ROOT_PATH}/3rdParty/libevent-2.1.6-beta/include")
include_directories("$(PROJECT_ROOT_PATH}/3rdParty/libevent-2.1.6-beta/compat")
include_directories("${PROJECT_ROOT_PATH}/src/common/src")
include_directories("${PROJECT_ROOT_PATH}/src/net/src")


if (MSVC)
	set(CMAKE_EXE_LINKER_FLAGS	"${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:\"LIBCMT\"")
endif()


add_executable(${PROJECT_NAME} ${WEBSPY_SRC} ${WEBSPY_HEADER} ${WEBSPY_BASIC} ${WEBSPY_HTTP_SRC})			# set the source files
add_dependencies(${PROJECT_NAME} common net)						# set the dependency libraries
target_link_libraries(${PROJECT_NAME} libevent_core libevent_extras common net)
target_link_libraries(${PROJECT_NAME} popt log4cplus.lib )			# link the libraries

file(GLOB_RECURSE SRC_HTTP	"${WEBSPY_SRC_FOLD}/http/*.cpp"
							"${WEBSPY_SRC_FOLD}/http/*.h")	
file(GLOB_RECURSE SRC_TASK	"${WEBSPY_SRC_FOLD}/task/*.cpp"
							"${WEBSPY_SRC_FOLD}/task/*.h")							
file(GLOB_RECURSE SRC_BASIC	${WEBSPY_BASIC})
							
add_library(${PROJECT_NAME}_HTTP 	${SRC_HTTP})
add_library(${PROJECT_NAME}_BASIC	${SRC_BASIC})
add_library(${PROJECT_NAME}_TASK	${SRC_TASK})

source_group("http"		FILES 	${SRC_HTTP})
source_group("basic" 	FILES 	${SRC_BASIC})
source_group("task" 	FILES 	${SRC_TASK})

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER app)	# set the project under folder app