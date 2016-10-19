#!/bin/bash

JDG_VERSION=7.0.0
MILESTONE_VERSION=DR1
BUILD_DIR=build

wget -N http://download.eng.bos.redhat.com/devel/candidates/JDG/JDG-$JDG_VERSION-$MILESTONE_VERSION/jboss-datagrid-$JDG_VERSION.$MILESTONE_VERSION-server.zip

rm -rf jboss-datagrid-${INFINISPAN_VERSION}
unzip -q jboss-datagrid-$JDG_VERSION.$MILESTONE_VERSION-server.zip
export JBOSS_HOME=`pwd`/infinispan-server-${INFINISPAN_VERSION}
cp test/data/* $JBOSS_HOME/standalone/configuration

if [  "${PROTOBUF_LIBRARY}" != "" ]
then
  CMAKE_EXTRAS="${CMAKE_EXTRAS} -DPROTOBUF_LIBRARY=${PROTOBUF_LIBRARY}"
fi

if [  "${PROTOBUF_PROTOC_LIBRARY}" != "" ]
then
  CMAKE_EXTRAS="${CMAKE_EXTRAS} -DPROTOBUF_PROTOC_LIBRARY=${PROTOBUF_PROTOC_LIBRARY}"
fi

if [  "$PROTOBUF_PROTOC_EXECUTABLE" != "" ]
then
  CMAKE_EXTRAS="${CMAKE_EXTRAS} -DPROTOBUF_PROTOC_EXECUTABLE=${PROTOBUF_PROTOC_EXECUTABLE}"
fi

if [  "$PROTOBUF_INCLUDE_DIR" != "" ]
then
  CMAKE_EXTRAS="${CMAKE_EXTRAS} -DPROTOBUF_INCLUDE_DIR=${PROTOBUF_INCLUDE_DIR}"
fi

if [  "$1" == "DEBUG" ]
then
  rm -rf ${BUILD_DIR} &&
  mkdir ${BUILD_DIR} &&
  cd ${BUILD_DIR} &&
  echo cmake -DCMAKE_BUILD_TYPE=Debug ${CMAKE_EXTRAS}.. &&
  cmake -DCMAKE_BUILD_TYPE=Debug ${CMAKE_EXTRAS}.. &&
  cmake --build .
else
  rm -rf ${BUILD_DIR} &&
  mkdir ${BUILD_DIR} &&
  cd ${BUILD_DIR} &&
  echo cmake ${CMAKE_EXTRAS} .. &&
  cmake ${CMAKE_EXTRAS} .. &&
  cmake --build . &&
  ctest -V &&
  cpack -G RPM
fi
