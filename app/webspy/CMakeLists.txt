set(PROJECT_NAME	webspy)
project(${PROJECT_NAME})

set(WEBSPY_SRC_FOLD ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(WEBSPY_SRC 			src/main.cpp
						src/basic_manager.cpp
						src/dns_manager.cpp
						src/web_voyager.cpp
						src/web_server.cpp)

set(WEBSPY_HEADER		src/basic_manager.h
						src/dns_manager.h
						src/web_voyager.h
						src/web_server.h)

set(WEBSPY_BASIC_SRC	src/config.h
						src/global.h
						src/global.cpp
						src/structure.h
						src/memory_creator.hpp)					

file(GLOB_RECURSE WEBSPY_HTTP_SRC	"${WEBSPY_SRC_FOLD}/http/*.cpp"
									"${WEBSPY_SRC_FOLD}/http/*.h")	
file(GLOB_RECURSE WEBSPY_TASK_SRC	"${WEBSPY_SRC_FOLD}/task/*.cpp"
									"${WEBSPY_SRC_FOLD}/task/*.h")
									
									
if (MSVC)
	set(CMAKE_EXE_LINKER_FLAGS	"${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:\"LIBCMT\"")
endif()		


include_directories("${PROJECT_ROOT_PATH}/3rdParty/libevent-2.1.6-beta")
include_directories("${PROJECT_ROOT_PATH}/3rdParty/libevent-2.1.6-beta/include")
include_directories("${PROJECT_ROOT_PATH}/3rdParty/libevent-2.1.6-beta/compat")
include_directories("${PROJECT_ROOT_PATH}/src/common/src")
include_directories("${PROJECT_ROOT_PATH}/src/net/src")

		
add_executable(${PROJECT_NAME} 	${WEBSPY_SRC} 
								${WEBSPY_HEADER} 
								${WEBSPY_BASIC_SRC} 
								${WEBSPY_HTTP_SRC} 
								${WEBSPY_TASK_SRC})				# set the source files
add_dependencies(${PROJECT_NAME} common net)				# set the dependency libraries

target_link_libraries(${PROJECT_NAME} 	libevent_core 
										libevent_extras 
										common 
										net
										popt
										log4cplus)
							
add_library(${PROJECT_NAME}_HTTP 	${WEBSPY_HTTP_SRC})
add_library(${PROJECT_NAME}_BASIC	${WEBSPY_BASIC_SRC})
add_library(${PROJECT_NAME}_TASK	${WEBSPY_TASK_SRC})


source_group("http"		FILES 	${WEBSPY_HTTP_SRC})
source_group("basic" 	FILES 	${WEBSPY_BASIC_SRC})
source_group("task" 	FILES 	${WEBSPY_TASK_SRC})

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER app)	# set the project under folder app