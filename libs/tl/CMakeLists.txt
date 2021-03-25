project(tl)

set(INC ${PROJECT_SOURCE_DIR})
set(HEADERS
	int_types.hpp
	str.hpp
	fmt.hpp
	uptr.hpp
	pair.hpp
	span.hpp
	basic.hpp
	basic_math.hpp
	delegate.hpp
	endian.hpp
	move.hpp
	signal.hpp
	random.hpp
	type_traits/is_reference.hpp
	type_traits/add_reference.hpp
	type_traits/declval.hpp
	type_traits/remove_reference.hpp
	type_traits/is_same_type.hpp
	type_traits/conditional_type.hpp
	type_traits/type_at.hpp
	type_traits/type_index.hpp
	containers/array.hpp
	containers/vector.hpp
	containers/fvector.hpp
	containers/hash_map.hpp
	containers/tuple.hpp
	containers/table.hpp
	hash/hash.hpp
	hash/str.hpp
	algorithms/copy.hpp
	algorithms/fill.hpp
	rect.hpp
	bitset.hpp
)
PREPEND(HEADERS "${INC}/tl" ${HEADERS})

set(SRC ${PROJECT_SOURCE_DIR})
set(SOURCES
	str.cpp
	fmt.cpp
	endian.cpp
	random.cpp
	pcg_basic.h pcg_basic.c
	hash/hash.cpp
)
PREPEND(SOURCES "${SRC}/tl" ${SOURCES})

add_library(tl STATIC
	${INC}/tl.hpp
	${HEADERS}
	${SOURCES}
)

target_include_directories(tl PUBLIC  ${INC})

target_link_libraries(tl
	glm
)

#set_target_properties(tl PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
#cotire(tl)
