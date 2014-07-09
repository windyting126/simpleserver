#!/usr/bin/perl
use strict;
use warnings;

my $strRcDbName = "presource_peer_new";
my $strRcTbName = "t_rc_online_peer_list_temp";
my $nRcTableHashBuckLen = 11;

my $strMd5ConfFile = "md5.conf";
my %hashFileMd5;


my $nRet = &LoadMd5();
if ($nRet != 0)
{
	print "LoadMd5 fail\n";
	exit -1;
}

print "LoadBeginTime:".&GetLocalStringTime()."\n";
for (my $nA = 0; $nA < $nRcTableHashBuckLen; ++ $nA)
{
	my $strTbName = $strRcTbName."_$nA";
	my $strDumpSqlFileName = "$strTbName.sql";
	my $strDumpZipfileName = "$strTbName.zip";
	my $strDumpZipfileMd5Name = "$strDumpZipfileName.md5";
	`rm -rf $strDumpSqlFileName`;
	if (!(-f $strDumpZipfileName))
	{
		next;
	}
	if (!(-f $strDumpZipfileMd5Name))
	{
		next;
	}
	my $strCurMd5 = `cat $strDumpZipfileMd5Name`;
	chomp($strCurMd5);
	my $strMd5 =  `md5sum $strDumpZipfileName | awk '{print \$1}'`;
	chomp($strMd5);
	if ($strCurMd5 ne $strMd5)
	{
		print "$strDumpSqlFileName have wrong md5";
		last;	
	}
	if (defined($hashFileMd5{$strDumpZipfileName}))
	{
		if ($strMd5 eq $hashFileMd5{$strDumpZipfileName})
		{
			next;
		}
		else
		{
			$hashFileMd5{$strDumpZipfileName} = $strMd5;	
		}
	}
	else
	{
		$hashFileMd5{$strDumpZipfileName} = $strMd5;
	}
	print "Load File $strDumpZipfileName";	
	`rm -rf $strDumpSqlFileName`;
	`unzip $strDumpZipfileName`;
	`mysql -uroot $strRcDbName < $strDumpSqlFileName`;	
	`rm -rf $strDumpSqlFileName`;
}
print "LoadEndTime:".&GetLocalStringTime()."\n";

&SaveMd5();





sub LoadMd5()
{
	my $fdFile = open(MD5FILE, "<", $strMd5ConfFile);
	if (!defined($fdFile))
	{
		print "open $strMd5ConfFile Fail\n";
		return -1;	
	}
	while(my $strLine = <MD5FILE>)
	{
		chomp($strLine);
		if ($strLine eq '')
		{
			next;
		}
		my @arrLine = split(' ', $strLine);
		if (@arrLine != 2)
		{
			print "ErrorLine: $strLine\n";
			next;
		}
		$hashFileMd5{$arrLine[0]} = $arrLine[1];
	}
	close(MD5FILE);
	return 0;
}

sub SaveMd5()
{
	my $fdFile = open(MD5FILE, ">", $strMd5ConfFile);
	if (!defined($fdFile))
	{
		print "open $strMd5ConfFile fail\n";return -1;
		return -1;
	}
	foreach my $strFileName (sort keys %hashFileMd5)
	{
		print MD5FILE "$strFileName $hashFileMd5{$strFileName}\n";
	}

	close(MD5FILE);
	return 0;
}

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
