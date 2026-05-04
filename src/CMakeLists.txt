cmake_minimum_required(VERSION 3.13.0)
project(srcmain)
aux_source_directory(com COM_SRCS)
aux_source_directory(conf CONF_SRCS)
aux_source_directory(fun FUN_SRCS)
aux_source_directory(sub/parser PARSER_SRCS)
aux_source_directory(stream  STREAM_SRCS)
aux_source_directory(file  FILE_SRCS)
aux_source_directory(file/Ctrldata  CTRLDATA_SRCS)
aux_source_directory(file/Example  EXAMPLE_SRCS)
aux_source_directory(log  LOG_SRCS)
aux_source_directory(struct  STRUCT_SRCS)

SET(SRC_LIST 
main.cc
${COM_SRCS}
${CONF_SRCS}
${FUN_SRCS}
${PARSER_SRCS}
${STREAM_SRCS}
${FILE_SRCS}
${CTRLDATA_SRCS}
${EXAMPLE_SRCS}
${LOG_SRCS}
${STRUCT_SRCS}
)

SET(LIB_LIST
xerces-c.a
libcurl.a
libACE.a
dl
libboost_date_time.a
libboost_regex.a
libuci.a
libubox.a
libcjson.a
libpcap.a
libjemalloc.a
libprotobuf.a
libevent.a
libflatbuffers.a
libparquet.a
libarrow.a
libarrow_bundled_dependencies.a
pthread
libgmp.a
libShuffle.a
)
set(CMAKE_CXX_FLAGS
	"${CMAKE_CXX_FLAGS} -std=c++14 -g  -Bstatic -Wall  -Wextra -Wno-return-local-addr -Wno-unused-variable -Wno-format-truncation -Wno-unused-result -Wno-format-overflow -Wno-maybe-uninitialized -Wno-unused-parameter -Wno-address -Wno-deprecated -Wno-parentheses -Wno-ignored-qualifiers -Wno-unused-but-set-variable -Wno-class-memaccess -Wno-missing-field-initializers -D__LITTLE_ENDIAN__ -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")

SET(LINK_LIB_DIRECTORY
${ace_LIBRARY_DIRS}
${curl_LIBRARY_DIRS}
${xerces-c_LIBRARY_DIRS}
${cjson_LIBRARY_DIRS}
${libpcap_LIBRARY_DIRS}
${libevent_LIBRARY_DIRS}
${gmp_LIBRARY_DIRS}
${protocol_LIBRARY_DIRS}    
${flatbuffers_LIBRARY_DIRS} 
${arrow_LIBRARY_DIRS}
${PROJECT_SOURCE_DIR}/../third-party/stacks/jemalloc/lib
${PROJECT_SOURCE_DIR}/../third-party/lib
${PROJECT_SOURCE_DIR}/../third-party/stacks/uci/lib
${PROJECT_SOURCE_DIR}/../third-party/stacks/ubox/lib
${PROJECT_SOURCE_DIR}/../third-party/stacks/boost_1_64_0/lib
${PROJECT_SOURCE_DIR}/../lib
)

include_directories(
${ace_INCLUDE_DIRS}
${curl_INCLUDE_DIRS}
${xerces-c_INCLUDE_DIRS}
${cjson_INCLUDE_DIRS}
${libpcap_INCLUDE_DIRS}
${libevent_INCLUDE_DIRS}
${gmp_INCLUDE_DIRS}
${protocol_INCLUDE_DIRS}   
${flatbuffers_INCLUDE_DIRS} 
${arrow_INCLUDE_DIRS}
${PROJECT_SOURCE_DIR}/../third-party/stacks/jemalloc/include
${PROJECT_SOURCE_DIR}/
${PROJECT_SOURCE_DIR}/../third-party/include/libxml2
${PROJECT_SOURCE_DIR}/../third-party/include
${PROJECT_SOURCE_DIR}/../third-party/stacks/ubox/include/libubox
${PROJECT_SOURCE_DIR}/../third-party/stacks/uci/include
${PROJECT_SOURCE_DIR}/../third-party/stacks/boost_1_64_0/include
${PROJECT_SOURCE_DIR}/../src/shuffle
)

LINK_DIRECTORIES(${LINK_LIB_DIRECTORY})

add_executable(litealigndata ${SRC_LIST})

target_link_libraries(litealigndata ${LIB_LIST})
add_custom_command(TARGET litealigndata
	POST_BUILD
	COMMAND rm -f ${PROJECT_SOURCE_DIR}/../testcase/litealigndata && rm -f ${PROJECT_SOURCE_DIR}/../testcase/case/Example &&
	rm -f ${PROJECT_SOURCE_DIR}/../testcase/case/Ctrldata &&
	ln -s ${PROJECT_SOURCE_DIR}/../build/src/litealigndata ${PROJECT_SOURCE_DIR}/../testcase/litealigndata && 
	ln -s ${PROJECT_SOURCE_DIR}/../seedfile/Example ${PROJECT_SOURCE_DIR}/../testcase/case/Example &&
	ln -s ${PROJECT_SOURCE_DIR}/../seedfile/Ctrldata ${PROJECT_SOURCE_DIR}/../testcase/case/Ctrldata
	)
