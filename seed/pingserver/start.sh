PRONAME=pingserver
CHECK=`ps -ef c | grep -o '[^ ]*$' | grep -o "^${PRONAME}$"`
if [ "${CHECK}" = "${PRONAME}" ]
then
    echo =================================================================================================================
    echo "Warning: Program [${PRONAME}] is running, please kill the running program or stop heartbeat first before start!" 
    echo =================================================================================================================
    exit 0
fi

rm -f /etc/cron.d/crontab_pingserver_file
/etc/init.d/crond restart > /dev/null

cd install
chmod +x ./server
cp -f ./server /etc/ha.d/resource.d/ &> /dev/null
if [ $? -ne 0 ]
then
	echo ======================================================================
	echo "Warning: Please install heartbeat first, and then do [`basename $0`]"
	echo ======================================================================
	exit 1
fi

echo "pingserver heartbeat script install ok!"
echo 
echo ============================================
echo "Please configure and then start heartbeat!"
echo ============================================
exit 0


