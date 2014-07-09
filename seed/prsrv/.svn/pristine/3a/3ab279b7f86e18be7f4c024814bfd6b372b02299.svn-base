CHECK=`ps -ef c | grep -o prsvr`
if [ "${CHECK}" = "prsvr" ]
then
    echo ===================================================================================
    echo "Warning : Program prsvr is running ,please kill the running program first before start!" 
    echo ===================================================================================
    exit 0
fi

ulimit -c unlimited
./prsvr &
cd install 
chmod +x *.sh 
dos2unix *
./crontab_install.sh 
echo check install prserver 
echo `ps -ef | grep prsvr | grep -v grep` 
