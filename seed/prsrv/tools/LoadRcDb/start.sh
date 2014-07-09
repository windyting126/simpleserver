#!/bin/bash
chmod +x *.sh *.pl
chmod 644 crontab_LoadRcTable
cp crontab_LoadRcTable /etc/cron.d/
touch /etc/cron.d/crontab_LoadRcTable
