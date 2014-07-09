#!/bin/bash
chmod 644 crontab_pconsrv_file
chown root crontab_pconsrv_file 
chgrp root crontab_pconsrv_file
cp crontab_pconsrv_file /etc/cron.d
touch /etc/cron.d/* 


