#!/bin/bash
PROJECT_DIR=`pwd`
STACKS_UNTAR_DIR=${PROJECT_DIR}/stacks/untar
THIRD_PARTY_STACKS_DIR=${PROJECT_DIR}/third-party/stacks
#----------------------------------------------------------------------
# remove all last building package
#----------------------------------------------------------------------
remove_building_package()
{
    rm -rf lib
    mkdir lib
    rm -rf ${STACKS_UNTAR_DIR}
    rm -rf ${THIRD_PARTY_STACKS_DIR}/*
    mkdir -p ${STACKS_UNTAR_DIR}
    cd ${STACKS_UNTAR_DIR}
    echo "End to remove all before building package ......"
}

#----------------------------------------------------------------------
# untar third package
#----------------------------------------------------------------------
untar_third_package()
{
    cd ${STACKS_UNTAR_DIR}
    echo "Start to untar third package ......"
    for file in $(find ../tarballs -type f)
    do
        tar xvf $file
    done
    echo "End to untar third package ......"
}

#----------------------------------------------------------------------
# Building jemalloc-5.2.0
#----------------------------------------------------------------------
building_third_package()
{
    cd ${STACKS_UNTAR_DIR}
    echo "Start to build jemalloc-5.2.0 library ......"
    cd jemalloc-5.2.0
    ./configure --prefix=${THIRD_PARTY_STACKS_DIR}/jemalloc --with-jemalloc-prefix=je_ 
    make -j4
    make install
    echo "End to build jemalloc-5.2.0 library ......"
    #----------------------------------------------------------------------
    # Building m4-1.4.18
    #----------------------------------------------------------------------
    echo "Start to build m4-1.4.18 library ......"
    cd ../m4-1.4.18
    ./configure --prefix=${THIRD_PARTY_STACKS_DIR}/m4
    make
    make install
    export PATH="$PATH:${THIRD_PARTY_STACKS_DIR}/m4/bin"
    echo "End to build m4-1.4.18 library ......"
    #----------------------------------------------------------------------
    # Building libtool-2.4.6
    #----------------------------------------------------------------------
    echo "Start to build libtool-2.4.6 library ......"
    cd ../libtool-2.4.6
    ./configure --prefix=${THIRD_PARTY_STACKS_DIR}/libtool
    make
    make install
    echo "End to build libtool-2.4.6 library ......"
    #----------------------------------------------------------------------
    # Building autoconf-2.71
    #----------------------------------------------------------------------
    echo "Start to build autoconf-2.71 library ......"
    cd ../autoconf-2.71
    ./configure  --prefix=${THIRD_PARTY_STACKS_DIR}/autoconf
    sed -i '2211,2221 s/^/#/' Makefile
    make
    make install
    export PATH="$PATH:${THIRD_PARTY_STACKS_DIR}/autoconf/bin"
    echo "End to build autoconf-2.71 library ......"
    #----------------------------------------------------------------------
    # Building gettext-0.22.3
    #----------------------------------------------------------------------
    echo "Start to build gettext-0.22.3 library ......"
    cd ../gettext-0.22.3
    ./configure  --prefix=${THIRD_PARTY_STACKS_DIR}/gettext
    make
    make install
    export PATH="$PATH:${THIRD_PARTY_STACKS_DIR}/gettext/bin"
    echo "End to build gettext-0.22.3 library ......"
    #----------------------------------------------------------------------
    # Building automake-1.16
    #----------------------------------------------------------------------
    echo "Start to build automake-1.16 library ......"
    cd ../automake-1.16
    ./bootstrap
    ./configure  --prefix=${THIRD_PARTY_STACKS_DIR}/automake
    make
    make install
    export PATH="$PATH:${THIRD_PARTY_STACKS_DIR}/automake/bin"
    echo "End to build automake-1.16 library ......"
    #----------------------------------------------------------------------
    # Building bison-3.8
    #----------------------------------------------------------------------
    echo "Start to build bison-3.8 library ......"
    cd ../bison-3.8
    ./configure --prefix=${THIRD_PARTY_STACKS_DIR}/bison
    make
    make install
    export PATH="$PATH:${THIRD_PARTY_STACKS_DIR}/bison/bin"
    echo "End to build bison-3.8 library ......"
    #----------------------------------------------------------------------
    # Building flex-2.6.3
    #----------------------------------------------------------------------
    echo "Start to build flex-2.6.3 library ......"
    cd ../flex-2.6.3
    export LIBTOOLIZE=${THIRD_PARTY_STACKS_DIR}/libtool/bin/libtoolize
    ./autogen.sh
    ./configure --prefix=${THIRD_PARTY_STACKS_DIR}/flex
    make
    make install
    export PATH="$PATH:${THIRD_PARTY_STACKS_DIR}/flex/bin"
    echo "End to build flex-2.6.3 library ......"
    #----------------------------------------------------------------------
    # Building libpcap-1.10
    #----------------------------------------------------------------------
    echo "Start to build libpcap-1.10 library ......"
    cd ../libpcap-1.10
    ./autogen.sh
    ./configure --prefix=${THIRD_PARTY_STACKS_DIR}/libpcap
    make
    make install
    echo "End to build libpcap-1.10 library ......"
    #----------------------------------------------------------------------
    # Building openssl-3.0.1
    #----------------------------------------------------------------------
    echo "Start to build openssl-3.0.1 library ......"
    cd ../openssl-3.0.1
    ./config --prefix=${THIRD_PARTY_STACKS_DIR}/openssl
    make -j4
    make install
    cd ${THIRD_PARTY_STACKS_DIR}/openssl
    ln -s lib64 lib
    cd -
    export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${THIRD_PARTY_STACKS_DIR}/openssl/lib
    echo "End to build openssl-3.0.1 library ......"
    #----------------------------------------------------------------------
    # Building curl-8.6.0
    #----------------------------------------------------------------------
    echo "Start to build curl-8.6.0 library ......"
    cd ../curl-8.6.0
    ./configure --prefix=${THIRD_PARTY_STACKS_DIR}/curl --with-openssl=${THIRD_PARTY_STACKS_DIR}/openssl --without-libpsl
    make -j4 
    make install
    echo "End to build curl-8.6.0 library ......"
    #----------------------------------------------------------------------
    # Building boost_1_64_0
    #----------------------------------------------------------------------
    echo "Start to build boost_1_64_0 library ......"
    BOOST_DIR=${THIRD_PARTY_STACKS_DIR}/boost_1_64_0
    cd ../boost_1_64_0
    ./bootstrap.sh --prefix=${THIRD_PARTY_STACKS_DIR}/boost_1_64_0 --with-toolset=gcc
    ./b2 install toolset=gcc --with-date_time --with-regex
    if  [ $? -ne 0 ];then
      echo "boost compiling failed!"
      exit
    fi
    echo "End to build boost_1_64_0 library ......"
    #----------------------------------------------------------------------
    # Building cmake-3.13.0
    # del Tests Modules and # some code from CMAKELIST.txt related it 
    # due to limit single file not more than 100MB on git rep
    #----------------------------------------------------------------------
    echo "Start to build CMake-3.26.2 library ......"
    cd ../CMake-3.26.2
    export OPENSSL_ROOT_DIR=${THIRD_PARTY_STACKS_DIR}/openssl
    ./bootstrap
    make -j4 
    make install DESTDIR=${THIRD_PARTY_STACKS_DIR}/cmake
    ALIGNDATA_CMAKE=${THIRD_PARTY_STACKS_DIR}/cmake/usr/local/bin/./cmake
    echo "End to build CMake-3.26.2 library ......"
    #----------------------------------------------------------------------
    # Building libubox
    #----------------------------------------------------------------------
    echo "Start to build libubox library ......"
    cd ../libubox
    ${ALIGNDATA_CMAKE} -DBUILD_LUA=off -DCMAKE_INSTALL_PREFIX=${THIRD_PARTY_STACKS_DIR}/ubox
    make install
    export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:${THIRD_PARTY_STACKS_DIR}/ubox/include/libubox
    export LIBRARY_PATH=$LIBRARY_PATH:${THIRD_PARTY_STACKS_DIR}/ubox/lib
    echo "End to build libubox library ......"
    #----------------------------------------------------------------------
    # Building uci
    #----------------------------------------------------------------------
    echo "Start to build uci library ......"
    cd ../uci
    ${ALIGNDATA_CMAKE} -DBUILD_LUA=off -DBUILD_STATIC=ON -DCMAKE_INSTALL_PREFIX=${THIRD_PARTY_STACKS_DIR}/uci
    make install
    export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:${THIRD_PARTY_STACKS_DIR}/uci/include
    export LIBRARY_PATH=$LIBRARY_PATH:${THIRD_PARTY_STACKS_DIR}/uci/lib
    echo "End to build uci library ......"
    #----------------------------------------------------------------------
    # Building xerces-c-3.1.4
    #----------------------------------------------------------------------
    echo "Start to build xerces-c-3.1.4 library ......"
    cd ../xerces-c-3.1.4
    ./configure --prefix=${THIRD_PARTY_STACKS_DIR}/xerces
    make -j8
    if  [ $? -ne 0 ];then
      echo "xerces compiling failed!"
      exit
    fi
    make install
    echo "End to build xerces-c-3.1.4 library ......"
    #----------------------------------------------------------------------
    # Building ACE_wrappers
    #----------------------------------------------------------------------
    echo "Start to build ACE_wrappers library ......"
    cd ../ACE_wrappers
    export ACE_ROOT=`pwd`
    cd $ACE_ROOT/ace
    cp config-linux.h config.h
    echo "include \$(ACE_ROOT)/include/makeinclude/platform_linux.GNU" > "${ACE_ROOT}/include/makeinclude/platform_macros.GNU"
    export INSTALL_PREFIX=${THIRD_PARTY_STACKS_DIR}/ace
    make static_libs_only=1 -j8
    make install
    echo "End to build ACE_wrappers library ......"
    #----------------------------------------------------------------------
    # Building cJSON
    #----------------------------------------------------------------------
    echo "Start to build cJSON library ......"
    cd ${STACKS_UNTAR_DIR}/cJSON
    mkdir build
    cd build
    ${ALIGNDATA_CMAKE} .. -DBUILD_SHARED_AND_STATIC_LIBS=On -DENABLE_CJSON_TEST=Off -DCMAKE_INSTALL_PREFIX=${THIRD_PARTY_STACKS_DIR}/cJSON
    make
    make install
    echo "End to build cJSON library ......"
    #----------------------------------------------------------------------
    # Building googletest
    #----------------------------------------------------------------------
    echo "Start to build googletest library ......"
    cd ${STACKS_UNTAR_DIR}/googletest
    mkdir build
    cd build
    ${ALIGNDATA_CMAKE} ../ -DCMAKE_INSTALL_PREFIX=${THIRD_PARTY_STACKS_DIR}/googletest
    make -j8
    make install
    echo "End to build googletest library ......"

    #----------------------------------------------------------------------
    # Building libevent-2.1.12-stable
    #----------------------------------------------------------------------
    echo "Start to build libevent library ......"
    cd ${STACKS_UNTAR_DIR}/libevent-2.1.12-stable
    mkdir build
    cd build
    ${ALIGNDATA_CMAKE} ../ -DCMAKE_INSTALL_PREFIX=${THIRD_PARTY_STACKS_DIR}/libevent
    make -j8
    make install
    echo "End to build libevent library ......"
    #----------------------------------------------------------------------
    # Building protobuf
    #----------------------------------------------------------------------
    echo "Start to build protobuf library ......"
    cd ${STACKS_UNTAR_DIR}/protobuf
    mkdir build
    cd build
    ${ALIGNDATA_CMAKE} ../ -Dprotobuf_BUILD_SHARED_LIBS=OFF -Dprotobuf_BUILD_STATIC_LIBS=ON -DCMAKE_INSTALL_PREFIX=${THIRD_PARTY_STACKS_DIR}/protobuf -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_WITH_ZLIB=OFF ../ -DCMAKE_CXX_FLAGS=-fPIC -DABSL_ROOT_DIR=${STACKS_UNTAR_DIR}/abseil-cpp -DABSL_PROPAGATE_CXX_STD=ON
    make -j4
    make install
    echo "End to build protobuf library ......"
    #----------------------------------------------------------------------
    # Building parquet lib
    #del go Module due to limit single file no more than 100MB on git rep
    #----------------------------------------------------------------------
    echo "Start to build parquet library ......"
    cd ${STACKS_UNTAR_DIR}/arrow/cpp
    mkdir build
    cd build
    ${ALIGNDATA_CMAKE} ../ -DCMAKE_INSTALL_PREFIX=${THIRD_PARTY_STACKS_DIR}/arrow -DARROW_PARQUET=ON -DARROW_OPTIONAL_INSTALL=ON -DARROW_BUILD_TESTS=ON ../
    make parquet
    make -j4
    make install
    echo "End to build parquet library ......"
    #----------------------------------------------------------------------
    # Building flatbuffers lib
    #----------------------------------------------------------------------
    echo "Start to build flatbuffers library ......"
    cd ${STACKS_UNTAR_DIR}/flatbuffers
    mkdir build
    cd build
    ${ALIGNDATA_CMAKE} ../ -DCMAKE_INSTALL_PREFIX=${THIRD_PARTY_STACKS_DIR}/flatbuffers
    make -j4
    make install
    echo "End to build flatbuffers library ......"
    #----------------------------------------------------------------------
    # Building texinfo-6.8 lib
    #----------------------------------------------------------------------
    echo "Start to build texinfo-6.8 library ......"
    cd ${STACKS_UNTAR_DIR}/texinfo-6.8
    ./configure --prefix=${THIRD_PARTY_STACKS_DIR}/texinfo
    make -j4
    make install
    export PATH="$PATH:${THIRD_PARTY_STACKS_DIR}/texinfo/bin"
    echo "End to build texinfo-6.8 library ......"
    #----------------------------------------------------------------------
    # Building help2man-1.49.3 lib
    #----------------------------------------------------------------------
    echo "Start to build help2man-1.49.3 library ......"
    cd ${STACKS_UNTAR_DIR}/help2man-1.49.3
    ./configure --prefix=${THIRD_PARTY_STACKS_DIR}/help2man
    make -j4
    make install
    export PATH="$PATH:${THIRD_PARTY_STACKS_DIR}/help2man/bin"
    echo "End to build help2man-1.49.3 library ......"
    #----------------------------------------------------------------------
    # Building gawk-5.3.0 lib
    #----------------------------------------------------------------------
    echo "Start to build gawk-5.3.0 library ......"
    cd ${STACKS_UNTAR_DIR}/gawk-5.3.0
    ./configure --prefix=${THIRD_PARTY_STACKS_DIR}/gawk
    make -j4
    make install
    export PATH="$PATH:${THIRD_PARTY_STACKS_DIR}/gawk/bin"
    echo "End to build gawk-5.3.0 library ......"
    #----------------------------------------------------------------------
    # Building gmp-6.3.0 lib
    #----------------------------------------------------------------------
    echo "Start to build gmp library ......"
    cd ${STACKS_UNTAR_DIR}/gmp-6.3.0
    ./configure --prefix=${THIRD_PARTY_STACKS_DIR}/gmp
    make -j4
    #make check
    make install
    echo "End to build gmp library ......"
}
    #----------------------------------------------------------------------
    # Building litealigndata/litealigndata-package
    #----------------------------------------------------------------------
building_litealigndata()
{ 
    echo "Start to Make package ......"
    echo "Start to build litealigndata ......"
    rm -rf ${PROJECT_DIR}/build
    mkdir ${PROJECT_DIR}/build
    cd ${PROJECT_DIR}/build
    ${ALIGNDATA_CMAKE}  .. -DCMAKE_INSTALL_PREFIX=${PROJECT_DIR}/litealigndata-package
    make -j8

    if  [ $? -ne 0 ];then
      echo "litealigndata compiling failed!"
      exit
    fi
    strip src/litealigndata && chmod +x src/litealigndata
    make install
    echo "End to build litealigndata ......"
    cd ${PROJECT_DIR}
    tar jcvf litealigndata-package.tar.bz2 litealigndata-package/
    rm -rf litealigndata-package/
    echo "End to Make package ......"
}

remove_building_package
untar_third_package
building_third_package
building_litealigndata
