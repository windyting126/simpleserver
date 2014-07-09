CHECK=`ps -ef c | grep -o pcon`
if [ "${CHECK}" = "pcon" ]
then
    echo ===================================================================================
    echo "Warning : Program pcon is running ,please kill the running program first before start!"
    echo ===================================================================================
    exit 0
fi 

ulimit -c unlimited
ulimit -n 102400
./pcon &
cd install 
chmod +x *.sh 
dos2unix *
./crontab_install.sh 
echo `ps -ef | grep pcon | grep -v grep`
