#
# net is a static library contains the most basic network functions
#
project(net)

#
# include the source files
#
set(NET_SRC 	src/inetaddr.cpp
				src/socket_base.cpp
				src/socket_tcp.cpp
				src/socket_udp.cpp)
set(NET_HEADER 	src/inetaddr.h
				src/socket_base.h
				src/protocol.h
				src/protocol_port.h)

add_library(net STATIC ${NET_SRC} ${NET_HEADER})
add_dependencies(net common)
target_link_libraries(net common)
set_target_properties (net PROPERTIES FOLDER lib)

#
# this project is to test net library
#
project(net_test)

set(NET_TEST_SRC	test/main.cpp
					test/inetaddr_test.cpp
					${PROJECT_ROOT_PATH}/test/incl/global.h)

add_executable(net_test ${NET_TEST_SRC})
add_dependencies(net_test net)
target_link_libraries(net_test net gtest popt log4cplus)

set_target_properties (net_test PROPERTIES FOLDER test)