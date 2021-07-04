cmake_minimum_required (VERSION 3.16)

set(MODULE_NAME tepp)

project(${MODULE_NAME} LANGUAGES CXX)

add_library(${MODULE_NAME}
	tepp.cpp include/tepp/tepp.h
	bind.cpp include/tepp/bind.h
)

target_include_directories(${MODULE_NAME} PUBLIC include)
target_include_directories(${MODULE_NAME} PRIVATE include/${MODULE_NAME})

target_compile_features(${MODULE_NAME} PUBLIC cxx_std_20)

add_executable(test_ops
	main.cpp
)

target_compile_features(test_ops PUBLIC cxx_std_20)

target_link_libraries(test_ops PUBLIC tepp)
