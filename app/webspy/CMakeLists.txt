project(webspy)

set(WEBSPY_SRC src/main.cpp)

include_directories("${PROJECT_ROOT_PATH}/3rdParty/libevent-2.0.21")
include_directories("${PROJECT_ROOT_PATH}/3rdParty/libevent-2.0.21/include")

add_executable(webspy ${WEBSPY_SRC})					# set the source files
add_dependencies(webspy common net)						# set the dependency libraries
target_link_libraries(webspy common net popt)			# link the libraries


set_target_properties (webspy PROPERTIES FOLDER app)	# set the project under folder app