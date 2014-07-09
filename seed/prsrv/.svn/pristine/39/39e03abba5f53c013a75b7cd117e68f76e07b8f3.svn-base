#!/usr/bin/perl
use strict;
use warnings;

my $strRcDbName = "presource_peer_new";
my $strRcTbName = "t_rc_online_peer_list_temp";
my $nRcTableHashBuckLen = 11;

my $strLocalUser = "rsync";
my $strBindIp = "0.0.0.0";
my $strDstHost = "218.60.3.167";
my $strDstPath = "/opt/LoadRcDb";


print "ScpBeginTime:".&GetLocalStringTime()."\n";
my $nRet = 0;
for (my $nA = 0; $nA < $nRcTableHashBuckLen; ++ $nA)
{
	my $strTbName = $strRcTbName."_$nA";
	my $strDumpSqlFileName = "$strTbName.sql";
	my $strDumpZipfileName = "$strTbName.zip";
	`mysqldump -uroot $strRcDbName $strTbName > $strDumpSqlFileName`;
	`zip -r $strDumpZipfileName $strDumpSqlFileName`;
	print "ScpBeginTime $nA:".&GetLocalStringTime()."\n";
	`su $strLocalUser -c "./rsync.sh $strDumpZipfileName $strDstPath $strDumpZipfileName $strBindIp $strDstHost"`;
	print "ScpEndTime $nA:".&GetLocalStringTime()."\n";
	$nRet = $?;
	if ($nRet != 0)
	{
       		print "rsync file $strDumpSqlFileName fail\n";
       		exit -1;
	}
	`rm  $strDumpSqlFileName $strDumpZipfileName`;
}
print "ScpEndTime:".&GetLocalStringTime()."\n";

sub GetLocalStringTime()
{
        my @arrLocalTime = localtime();
        $arrLocalTime[4] ++;
        $arrLocalTime[5] += 1900;
        my $strStringTime =  sprintf("%04d", $arrLocalTime[5])."-";
        $strStringTime .= sprintf("%02d", $arrLocalTime[4])."-";
        $strStringTime .= sprintf("%02d", $arrLocalTime[3])." ";
        $strStringTime .= sprintf("%02d", $arrLocalTime[2]).":";
        $strStringTime .= sprintf("%02d", $arrLocalTime[1]).":";
        $strStringTime .= sprintf("%02d", $arrLocalTime[0]);
        return $strStringTime;
}




