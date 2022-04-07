cmake_minimum_required (VERSION 3.16)

set(MODULE_NAME tepp)

project(${MODULE_NAME} LANGUAGES CXX)

make_module(
	MODULE_NAME ${MODULE_NAME}
	MODULE_FILES
		tepp
		dest
		enum_array
		enum_bitflags
		rt_export
		numeric_type
		angle
		heap_object
		rt_vector
		rt_aggregate
		cached_heap_object
		bi_map
		optional_ref
	CXX_STANDARD 23
)
