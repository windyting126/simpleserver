#!/usr/bin/perl
use strict;
use warnings;

my $strLocalUser = "rsync";
my $strBindIp = "0.0.0.0";

my $strDstHost = "218.60.3.162";
my $strDstUsr = "rsync";
my $strDstPath = "/opt/pingserver/install";
my $strDstFileName = "HashTable.txt.zip";


my $s_strSrcPath = "/opt/pingserver/install";
my $s_strDumpFile = "HashTable.txt";
my $s_strAbsDumpFile = "$s_strSrcPath/$s_strDumpFile";
my $s_strZipFile = "HashTable.txt.zip";
my $s_strAbsZipFile = "$s_strSrcPath/$s_strZipFile";

if (-e $s_strAbsDumpFile)
{
	`rm -rf $s_strAbsDumpFile`;
}

print "DumpBeginTime:".&GetLocalStringTime()."\n";
`./DumpPingPeer 127.0.0.1 18000`;
my $nRet = $?;
print "DumpEndTime:".&GetLocalStringTime()."\n";

if ($nRet != 0)
{
	print "DumpPingPeer Fail\n";
	exit -1;
}

if (!(-f $s_strAbsDumpFile))
{
	print "DumpFile is not Exist\n";
	exit -1;
}
`chmod 0777 $s_strAbsDumpFile`;


`cd $s_strSrcPath;rm -rf $s_strZipFile`;
print "ZipBeginTime:".&GetLocalStringTime()."\n";
`cd $s_strSrcPath; zip $s_strZipFile $s_strDumpFile`;
print "ZipEndTime:".&GetLocalStringTime()."\n";

print "ScpBeginTime:".&GetLocalStringTime()."\n";
`su $strLocalUser -c "./rsync.sh $s_strAbsZipFile $strDstPath $strDstFileName $strBindIp $strDstHost"`;
$nRet = $?;
print "ScpEndTime:".&GetLocalStringTime()."\n";
if ($nRet != 0)
{
	print "rsync file fail\n";
	exit -1;
}
print "DumpAndRsync Suc\n";
exit 0;





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
