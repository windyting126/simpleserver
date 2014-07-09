#!/bin/bash
export SERVER_ROOT=`pwd`
cd ..
cd ..
cd ..
BASEROOT=`pwd`
cd ..
BASEBASEROOT=`pwd`
cd ..
BASEBASEBASEROOT=`pwd`
cd ${SERVER_ROOT}
export PUBLIC_DIR=${BASEBASEBASEROOT}/public
export FRAMECOMMON=${BASEBASEROOT}/framecommon
export SRVFRAME=${BASEBASEROOT}/srvframe
export COMMON=${BASEROOT}/common

