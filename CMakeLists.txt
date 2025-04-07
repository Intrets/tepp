cmake_minimum_required (VERSION 3.16)

set(MODULE_NAME tepp)

project(${MODULE_NAME} LANGUAGES CXX)

make_module(
	MODULE_NAME ${MODULE_NAME}
	MODULE_FILES
		optional
		any
		name_thread
		rt_export_buffered_value
		unordered_map
		trivial_buffer
		arena
		maybe
		make_unique
		member_helper_macros
		try_catch
		in_interval
		format_exception
		safety_cast
		assert
        enumerate
		integers
		misc
		temp_format
		defer
		tepp
		dest
		enum_array
		enum_bitflags
		rt_export
		rt_export_size
		rt_ring_export
		numeric_type
		angle
		heap_object
		rt_vector2
		rt_aggregate_dynamic
		rt_base
		rt_value2
		rt_move_value2
		rt_queue2
		cached_heap_object
		bi_map
		rt_queue2
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
		uuid
		limit_finder
		functional
		pmr_container
		ring_buffer
		hash
		polyant
		unique_object
	CXX_STANDARD 23
)

add_executable(polyanttest polyanttest.cpp)
target_link_libraries(polyanttest PUBLIC tepp)
