cmake_minimum_required (VERSION 3.16)

set(MODULE_NAME tepp)

project(${MODULE_NAME} LANGUAGES CXX)

make_module(
	MODULE_NAME ${MODULE_NAME}
	MODULE_FILES
		misc
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
		rt_value
		cached_heap_object
		bi_map
		optional_ref
		switched_vector
		call_once
		intrusive_list
		string_literal
		atomic_value
		indexed_vector
	CXX_STANDARD 23
)
