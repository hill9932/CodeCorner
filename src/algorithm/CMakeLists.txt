#
# algm is a static library contains the most basic algorithm and data structure
#
set(PROJECT_NAME	algm)
project(${PROJECT_NAME})

include_directories("${PROJECT_ROOT_PATH}/src/algorithm/src")

#
# include the source files
#
set(SOURCES 	src/trie_tree.cpp)
set(HEADERS 	src/trie_tree.h
				src/rb_tree.h
				src/rb_tree.hxx
				src/bsearch_tree.h
				src/bsearch_tree.hxx
				src/min_max_heap.h
				src/min_max_heap.hxx
				src/hash.hxx
				src/hash_map.h
				src/hash_map.hxx
				src/sort_array.h
				src/sort_array.hxx)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
add_dependencies(${PROJECT_NAME} common)
target_link_libraries(${PROJECT_NAME} common)

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER lib)	# set the project under folder app


#
# this project is to test algorithm library
#
set(PROJECT_TEST_NAME	algm_test)
project(${PROJECT_TEST_NAME})

set(TEST_SOURCES	test/main.cpp
					test/trie_tree_test.cpp
					test/rb_tree_test.cpp
					test/bsearch_tree_test.cpp
					test/heap_test.cpp
					test/hash_map_test.cpp
					test/sort_array_test.cpp)

add_executable(${PROJECT_TEST_NAME} ${TEST_SOURCES})
add_dependencies(${PROJECT_TEST_NAME} ${PROJECT_NAME})
target_link_libraries(${PROJECT_TEST_NAME} algm gtest popt log4cplus)

set_target_properties (${PROJECT_TEST_NAME} PROPERTIES FOLDER test)