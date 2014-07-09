#!/bin/bash 
export SERVER_ROOT=`pwd`
cd ..
BASEROOT=`pwd`
cd ..
BASEBASEROOT=`pwd`
cd ${SERVER_ROOT}
export PUBLIC_DIR=${BASEBASEROOT}/../../public
export FRAMECOMMON=${BASEROOT}/../../framecommon
export SRVFRAME=${BASEROOT}/../../srvframe

if [ $# == 0 ];then
        make
elif [ $# == 1 ];then
        if [ $1 == 'clean' ];then
                make clean
        else
			export FILENAME=$1 
			export BINARY=`echo ${FILENAME} | awk -F'.' '{print $1}'`
			make -f makefile.one
        fi
fi
