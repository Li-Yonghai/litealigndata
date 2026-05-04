project(DeEncodeCtrl)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/com COM_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/conf CONF_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/fun FUN_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/sub/parser PARSER_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/stream  STREAM_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/file  FILE_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/file/Ctrldata  CTRLDATA_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/file/Example  EXAMPLE_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/log  LOG_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/struct  STRUCT_SRCS)

SET(SRC_LIST 
main.cc
${COM_SRCS}
${CONF_SRCS}
${FUN_SRCS}
${PARSER_SRCS}
${SHUFFLE_SRCS}
${STREAM_SRCS}
${FILE_SRCS}
${CTRLDATA_SRCS}
${EXAMPLE_SRCS}
${LOG_SRCS}
${STRUCT_SRCS}   
${CASEEXAMPLE_SRC}
)

SET(LIB_LIST
libevent.a
libxerces-c.a
libACE.a
dl
libboost_date_time.a
libboost_regex.a
libcjson.a
pthread
libgmp.a
libShuffle.a
)

set(CMAKE_CXX_FLAGS
	"${CMAKE_CXX_FLAGS} -std=c++14 -g  -Bstatic -Wall  -Wextra -Wno-return-local-addr -Wno-unused-variable -Wno-format-truncation -Wno-unused-result -Wno-format-overflow -Wno-maybe-uninitialized -Wno-unused-parameter -Wno-address -Wno-deprecated -Wno-parentheses -Wno-ignored-qualifiers -Wno-unused-but-set-variable -Wno-missing-field-initializers -D__LITTLE_ENDIAN__ -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")

SET(LINK_LIB_DIRECTORY 
${ace_LIBRARY_DIRS}
${gmp_LIBRARY_DIRS}
${xerces-c_LIBRARY_DIRS}
${libevent_LIBRARY_DIRS}
${cjson_LIBRARY_DIRS}
${PROJECT_SOURCE_DIR}/../../../third-party/stacks/boost_1_64_0/lib
${CMAKE_CURRENT_SOURCE_DIR}/../../../lib
)

include_directories(
${ace_INCLUDE_DIRS}
${gmp_INCLUDE_DIRS}
${xerces-c_INCLUDE_DIRS}
${libevent_INCLUDE_DIRS}
${cjson_INCLUDE_DIRS}
${PROJECT_SOURCE_DIR}/../../../src
${PROJECT_SOURCE_DIR}/../../../src/sub
${PROJECT_SOURCE_DIR}/../../../third-party/stacks/boost_1_64_0/include
${PROJECT_SOURCE_DIR}/../../../src/shuffle
)

LINK_DIRECTORIES(${LINK_LIB_DIRECTORY})

add_executable(litealigndataDeEncodeCtrl ${SRC_LIST})

target_link_libraries(litealigndataDeEncodeCtrl ${LIB_LIST})
add_custom_command(TARGET litealigndataDeEncodeCtrl
	POST_BUILD
	COMMAND rm -f ${PROJECT_SOURCE_DIR}/../../litealigndataDeEncodeCtrl &&
	cp litealigndataDeEncodeCtrl ${PROJECT_SOURCE_DIR}/../../
	)
