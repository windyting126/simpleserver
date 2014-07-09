#!/bin/bash
chmod +x *.sh *.pl
chmod +x LoadPingPeer
chmod 644 crontab_LoadPeerInfo
cp crontab_LoadPeerInfo /etc/cron.d/
touch /etc/cron.d/crontab_LoadPeerInfo
