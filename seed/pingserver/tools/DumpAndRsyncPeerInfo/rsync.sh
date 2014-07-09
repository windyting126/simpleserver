#!/bin/bash 
function print_ok() {
	echo -n OK
}
function print_err()
{
	echo -n ERROR
}

if [ $# -ne 5 ]
then
	echo Usage:$0 srcfilename destpath destfilename bindip desthost 
	print_err
	exit -1;
fi


#DST_HOST is P2P SrcServer
#DST_HOST=117.79.85.163
SRC_FILE_NAME=$1
DEST_PATH=$2
DEST_FILE_NAME=$3
BIND_IP=$4
DEST_HOST=$5
#SPEED_LIMIT umit Kbib/s
SPEED_LIMIT=16384

#if you want use another DST_USR, please modify the DST_USR values below

DST_USR=rsync

if [ ! -e $SRC_FILE_NAME ]
then
    echo srcfile is not exist
	
	exit 2
fi

REAL_DEST_FILE=$DEST_PATH/$DEST_FILE_NAME

MD5_SRC=`md5sum $SRC_FILE_NAME | awk '{printf("%s", $1)}'`

DEST_FILE_NAME_TMP=$REAL_DEST_FILE".tmp" 
MD5_LAST_DST=`ssh $DST_USR@$DEST_HOST "if [ -e $REAL_DEST_FILE ]; then md5sum $REAL_DEST_FILE | awk '{printf(\"%s\", \\$1)}'; fi"`
if [ $? -eq 0 ]
then
	if [ "$MD5_SRC" = "$MD5_LAST_DST" ]
	then
		echo "the file you want rsync is the same"
		exit 0
	fi	
fi

scp -C -p -l $SPEED_LIMIT -o BindAddress=$BIND_IP -o CompressionLevel=9 $SRC_FILE_NAME $DST_USR@$DEST_HOST:$DEST_FILE_NAME_TMP 
if [ $? -ne 0 ]
then
	print_err
	exit 3
fi

MD5_SRC_DST=`ssh $DST_USR@$DEST_HOST md5sum $DEST_FILE_NAME_TMP | awk '{printf("%s", $1)}'`

if [ "$MD5_SRC"  != "$MD5_SRC_DST" ] 
then
	echo src and dest md5 is not corret
	echo $MD5_SRC
	echo $MD5_SRC_DST
	exit 4
fi
echo md5 compare is correct
mvRet=`ssh $DST_USR@$DEST_HOST mv $DEST_FILE_NAME_TMP $REAL_DEST_FILE`
DEST_MD5_FILE=$REAL_DEST_FILE".md5"
#echo $DEST_MD5_FILE
md5ret=`ssh $DST_USR@$DEST_HOST "echo $MD5_SRC_DST > $DEST_MD5_FILE"`

print_ok
exit 0

