#!/bin/bash
chmod +x *.sh *.pl
chmod +x DumpPingPeer safersync
chmod 644 crontab_DumpAndRsync
cp crontab_DumpAndRsync /etc/cron.d/ 
touch /etc/cron.d/crontab_DumpAndRsync
