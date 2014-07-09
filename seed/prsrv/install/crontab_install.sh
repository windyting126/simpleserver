#!/bin/bash
chmod 644 crontab_prsvr_file
chown root crontab_prsvr_file 
chgrp root crontab_prsvr_file
cp crontab_prsvr_file /etc/cron.d
touch /etc/cron.d/* 


