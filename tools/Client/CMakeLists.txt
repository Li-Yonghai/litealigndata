project(Client)
SET(SRC_LIST 
main.cc
Connector.cc
)

SET(LIB_LIST
dl
libevent.a
pthread
)
set(CMAKE_CXX_FLAGS
	"${CMAKE_CXX_FLAGS} -std=c++14 -g  -Bstatic -Wall  -Wextra -Wno-unused-variable -Wno-format-truncation -Wno-unused-result -Wno-format-overflow -Wno-maybe-uninitialized -Wno-unused-parameter -Wno-address -Wno-deprecated -Wno-parentheses -Wno-ignored-qualifiers -Wno-unused-but-set-variable -Wno-missing-field-initializers -D__LITTLE_ENDIAN__ -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")

SET(LINK_LIB_DIRECTORY 
${libevent_LIBRARY_DIRS}
)

include_directories(
${libevent_INCLUDE_DIRS}
)

LINK_DIRECTORIES(${LINK_LIB_DIRECTORY})

add_executable(litealigndataClient ${SRC_LIST})

target_link_libraries(litealigndataClient ${LIB_LIST})
add_custom_command(TARGET litealigndataClient
	POST_BUILD
	COMMAND rm -f ${PROJECT_SOURCE_DIR}/../litealigndataClient &&
	cp litealigndataClient ${PROJECT_SOURCE_DIR}/../
	)
