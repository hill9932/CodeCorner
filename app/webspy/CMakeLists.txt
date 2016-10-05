project(webspy)

set(WEBSPY_SRC 		src/main.cpp
					src/dns_manager.cpp)

set(WEBSPY_HEADER	src/dns_manager.h)


include_directories("${PROJECT_ROOT_PATH}/3rdParty/libevent-2.0.21")
include_directories("${PROJECT_ROOT_PATH}/3rdParty/libevent-2.0.21/include")
include_directories("${PROJECT_ROOT_PATH}/src/common/src")
include_directories("${PROJECT_ROOT_PATH}/src/net/src")

add_executable(webspy ${WEBSPY_SRC} ${WEBSPY_HEADER})					# set the source files
add_dependencies(webspy common net)						# set the dependency libraries
target_link_libraries(webspy libevent_core libevent_extras common net popt)			# link the libraries


set_target_properties (webspy PROPERTIES FOLDER app)	# set the project under folder app