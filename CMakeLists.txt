cmake_minimum_required (VERSION 3.16)

set(MODULE_NAME tepp)

project(${MODULE_NAME} LANGUAGES CXX)

add_library(${MODULE_NAME}
	tepp.cpp include/tepp/tepp.h
	dest.cpp include/tepp/dest.h
	enum_array.cpp include/tepp/enum_array.h
	enum_bitflags.cpp include/tepp/enum_bitflags.h
	rt_export.cpp include/tepp/rt_export.h
	numeric_type.cpp include/tepp/numeric_type.h
)

target_include_directories(${MODULE_NAME} PUBLIC include)
target_include_directories(${MODULE_NAME} PRIVATE include/${MODULE_NAME})

target_compile_features(${MODULE_NAME} PUBLIC cxx_std_23)
