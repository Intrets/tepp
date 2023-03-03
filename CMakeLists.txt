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
		rt_move_value
		rt_queue
		rt_storage
		cached_heap_object
		bi_map
		optional_ref
		switched_vector
		call_once
		intrusive_list
		string_literal
		literal
		atomic_value
		indexed_vector
		cheated_storage
		simple_vector
		mutex
		call_on_scope_exit
		variant
		binary_semaphore
		type_index
		cstring_view
	CXX_STANDARD 23
)
