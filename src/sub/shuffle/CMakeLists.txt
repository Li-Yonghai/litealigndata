cmake_minimum_required(VERSION 3.13.0)
project(Shuffle)
SET(SHUFFLE_SRCS Shuffle.cc)
ADD_LIBRARY(Shuffle STATIC ${SHUFFLE_SRCS})
