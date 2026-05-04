project(casetest)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/baseCase   BASECASE_SRC)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../src/com COM_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../src/conf CONF_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../src/fun FUN_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../src/sub/parser/ PARSER_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../src/stream  STREAM_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../src/file  FILE_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../src/file/Ctrldata  CTRLDATA_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../src/file/Example  EXAMPLE_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../src/log  LOG_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../../src/struct  STRUCT_SRCS)

SET(SRC_LIST
    casemain.cc
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
    ${BASECASE_SRC}
)

SET(LIB_LIST
        libevent.a
        xerces-c.a
        libgmock.a
        libgtest.a
        libuci.a
        libubox.a
        libcurl.a
        libACE.a
        libboost_date_time.a
        libboost_regex.a
        libcjson.a
        libpcap.a
        libjemalloc.a
        dl
        pthread
        libgmp.a
	libShuffle.a
)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -std=c++17 -g  -Bstatic -Wall -Werror -Wextra -Wno-return-local-addr -Wno-unused-but-set-parameter -Wno-write-strings -Wno-stringop-overflow -Wno-stringop-truncation -Wno-catch-value -Wno-misleading-indentation -Wno-implicit-fallthrough -Wno-unused-variable -Wno-unused-parameter -Wno-address -Wno-deprecated -Wno-parentheses -Wno-ignored-qualifiers -Wno-unused-but-set-variable -Wno-missing-field-initializers -D__LITTLE_ENDIAN__ -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-reAlloc -fno-builtin-free")

LINK_DIRECTORIES(
    ${ace_LIBRARY_DIRS}
    ${gtest_LIBRARY_DIRS}
    ${curl_LIBRARY_DIRS}
    ${xerces-c_LIBRARY_DIRS}
    ${cjson_LIBRARY_DIRS}
    ${libpcap_LIBRARY_DIRS}
    ${libevent_LIBRARY_DIRS}
    ${gmp_LIBRARY_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/stacks/jemalloc/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/stacks/uci/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/stacks/ubox/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/stacks/boost_1_64_0/lib
)

include_directories(
    ${ace_INCLUDE_DIRS}
    ${gtest_INCLUDE_DIRS}
    ${curl_INCLUDE_DIRS}
    ${xerces-c_INCLUDE_DIRS}
    ${cjson_INCLUDE_DIRS}
    ${libpcap_INCLUDE_DIRS}
    ${libevent_INCLUDE_DIRS}
    ${gmp_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/stacks/jemalloc/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/stacks/ubox/include/libubox
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/stacks/uci/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/stacks/boost_1_64_0/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/sub
    ${CMAKE_CURRENT_SOURCE_DIR}/Examplecase
)

add_executable(casemain ${SRC_LIST})
target_link_libraries(casemain ${LIB_LIST})

message("Making case1 testcase")
add_custom_command(TARGET casemain
    POST_BUILD
    COMMAND rm -f ${CMAKE_CURRENT_SOURCE_DIR}/casemain && cp -f casemain ${CMAKE_CURRENT_SOURCE_DIR}/ && 
    cd ${CMAKE_CURRENT_SOURCE_DIR}/ && rm -rf *output* *xml  *json && 
    ./casemain --gtest_repeat=1 --gtest_output="json:./casemain.json"
)
