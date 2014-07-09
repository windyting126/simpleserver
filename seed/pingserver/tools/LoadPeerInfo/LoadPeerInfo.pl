#!/usr/bin/perl
use strict;
use warnings;

my $strFileDir = "/opt/pingserver/install";
my $strPeerInfoFile = "HashTable.txt.zip";
my $strMd5File = "$strPeerInfoFile.md5";
my $strUnzipPeerInfo = "HashTable.txt";


my $strLastMd5File = "lastmd5.save";

my $strAbsPeerInfoFile = "$strFileDir/$strPeerInfoFile";
my $strAbsMd5File = "$strFileDir/$strMd5File";

if (!(-f $strAbsPeerInfoFile))
{
	print "$strAbsPeerInfoFile is not a file or not exist\n";
	exit -1;
}

if (!(-f $strAbsMd5File))
{
        print "$strAbsMd5File is not a file or not exist\n";
        exit -1;
}

my $strMd5 =  `cat $strAbsMd5File`;
chomp($strMd5);
if (-f $strLastMd5File)
{
	my $strLastMd5 = `cat $strLastMd5File`;
	chomp($strLastMd5);
	if ($strMd5 eq $strLastMd5)
	{
		#print "no change \n";
		exit 0;
	}
}

my $strCalcMd5 = `md5sum $strAbsPeerInfoFile | awk '{print \$1}'`;
chomp($strCalcMd5);
if ($strCalcMd5 ne $strMd5)
{
	print "$strAbsPeerInfoFile"."'s md5 is not right\n";
	exit -1;
}

`cd $strFileDir;rm -rf $strUnzipPeerInfo;`;
print "UnzipBeginTime:".&GetLocalStringTime()."\n";
`cd $strFileDir;unzip $strPeerInfoFile;`;
print "UnzipEndTime:".&GetLocalStringTime()."\n";

print "LoadBeginTime:".&GetLocalStringTime()."\n";
`./LoadPingPeer 127.0.0.1 18000`;
print "LoadEndTime:".&GetLocalStringTime()."\n";
my $nRet = $?;
if ($nRet != 0)
{
	print "LoadPingPeer fail\n";
	exit -1;
}

`echo $strCalcMd5 > $strLastMd5File`;
print "LoadPingPeer Suc\n";



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
