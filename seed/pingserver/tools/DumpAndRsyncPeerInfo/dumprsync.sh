#!/bin/bash
cd /opt/DumpAndRsyncPeerInfo
PROCNAME=DumpAndRsync.pl

PROCNAME_FIND=`ps -ef  | grep -o "./${PROCNAME}$" | grep -o "${PROCNAME}"`      

if [ "$PROCNAME" = "$PROCNAME_FIND" ]
then
	echo programe $PROCNAME is running
	exit -1
else
	echo Start DumpAndRsync.pl
	./${PROCNAME}&
fi

exit
