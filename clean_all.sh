#!/bin/bash
PROJECT_DIR=`pwd`
rm -rf lib

STACKS_UNTAR_DIR=${PROJECT_DIR}/stacks/untar
THIRD_PARTY_STACKS_DIR=${PROJECT_DIR}/third-party/stacks
#----------------------------------------------------------------------
# Clean litealigndata
#----------------------------------------------------------------------
rm -rf ${PROJECT_DIR}/build

cd ${PROJECT_DIR}/testcase
cd case
rm -rf *.json Ctrldata  *.xml  Example
rm -rf casemain Example CaseExample.json *.xml *output casemain.json
cd ../apitest
rm -rf apimain  output apimain.json
rm -f ../litealigndata
#rm -rf ${PROJECT_DIR}/stacks/untar/
rm -rf ${PROJECT_DIR}/litealigndata-package.tar.bz2
rm -rf ${STACKS_UNTAR_DIR}
rm -rf ${THIRD_PARTY_STACKS_DIR}/*
cd ${PROJECT_DIR}/tools
rm -rf litealigndataClient output litealigndataDeEncodeCtrl
