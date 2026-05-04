#!/bin/bash
[ -z $BUILD_CONTAINER ] && BUILD_CONTAINER=lyh
[ -z $WORKSPACE ] && WORKSPACE=workspace
[ -z $IMAGE_NAME ] && IMAGE_NAME=pip-app-compiler
[ -z $IMAGE_VERSION ] && IMAGE_VERSION=1.0.1
LOCAL_WORKSPACE=$(echo ~)/$WORKSPACE
INNER_WORKSPACE=/root/$WORKSPACE
sudo docker run -itd \
--name $BUILD_CONTAINER \
-v ${LOCAL_WORKSPACE}:${INNER_WORKSPACEE} \
-v /usr/bin/qemu-aarch64-static:/usr/bin/qemu-aarch64-static \
-w ${INNER_WORKSPACE} \
${IMAGE_NAME}:${IMAGE_VERSION} \
bash

via buid docker 

fun_build()
{
[ ! -x "build" ] && mkdir build
sudo docker exec -it $BUILD_CONTAINER bash -c "cd $ABS_PATH_DOCKER_CURR/build && cmake .. && make $*
}
