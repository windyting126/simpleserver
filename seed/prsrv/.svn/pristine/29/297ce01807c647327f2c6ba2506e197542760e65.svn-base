#!/usr/bin/perl
use strict;
use warnings;
use DBI;

if (@ARGV != 2)
{
	print "$0 SrcDbConf DstDbConf\n";
	exit -1;
}

my $g_strDbName = "presource_peer_new";
my $g_strUser = "root";
my $g_strPasswd = "epIuGpKGAsMhtle24j";
#my $g_strPasswd = ""; #内网测试用
my $g_strOnlineRcTbName = "t_rc_online_peer_list_temp";
my $g_strRcTbName = "t_rc_peer_list_temp";
my $g_nTableSplitNum = 11; 

my $g_nSrcDbNum = 3;	#为了安全起见，读取配置文件后用这个值校验一下
my $g_nDstDbNum = 7;	#为了安全起见，读取配置文件后用这个值校验一下

my $g_strSrcDbConf = $ARGV[0];
my $g_strDstDbConf = $ARGV[1];

my @g_arrHashSrcDb;
my @g_arrHashDstDb;

my @g_arrSrcDbHandle;
my @g_arrDstDbHandle;

my $nRet = &LoadSrcSrv();
if ($nRet != 0)
{
	print "LoadSrcSrv fail\n";
	exit -1;
}
$nRet = &LoadDstSrv();
if ($nRet != 0)
{
	print "LoadDstSrv fail\n";
	exit -1;
}

$nRet = &ConnectSrv();
if ($nRet != 0)
{
	print "ConnectSrv Fail\n";
	exit -1;
}

$nRet = &Redistribute();
if ($nRet != 0)
{
	print "Redistribute Fail\n";
	exit -1;
}
print "Redistribute succ\n";


sub Redistribute()
{
	print "Betin:".&GetLocalStringTime()."=========\n";
	for (my $nA = 0; $nA < $g_nSrcDbNum; ++ $nA)
	{
		for (my $nB = 0; $nB < $g_nTableSplitNum; ++ $nB)
		{
			print "\t$g_nSrcDbNum $g_nTableSplitNum : $nA $nB ===========\n";
			my $strQuery = "select * from $g_strDbName.t_rc_online_peer_list_temp_$nB;";
			my $sthMysql = &Execute($g_arrSrcDbHandle[$nA], $strQuery);
			if (!defined($sthMysql))
			{
				print "select * from $g_strDbName.t_rc_online_peer_list_temp_$nB fail\n";
				return -1;
			}
			my @arrRet;
			my %hashTmp = ();
			my $nIndex = 0;
			print "\tSelect $nA:$nB Begin:".&GetLocalStringTime()."=========\n";
			while(@arrRet = $sthMysql->fetchrow_array())
			{
				if (@arrRet != 8)
				{
					print "fetchrow from $g_strDbName.t_rc_online_peer_list_temp_$nB fail\n";
					return -1;
				}
				
				$hashTmp{$nIndex} = [@arrRet];
				++ $nIndex;
			}
			print "\tSelect $nA:$nB End:".&GetLocalStringTime()."=========\n";
			
			print "\tReDistribute $nA:$nB Begin:".&GetLocalStringTime()."=========\n";
			my %hashTmp2 = ();
			print "\t\tRedistribute Calc Begin:".&GetLocalStringTime()."=========\n";
			foreach my $nKey (keys %hashTmp)
			{
				my $nDstSrvIndex = &ShortELFHash( $hashTmp{$nKey}[0]) % $g_nDstDbNum;
				$hashTmp{$nKey}[0] = $g_arrSrcDbHandle[$nA]->quote($hashTmp{$nKey}[0]);
				$hashTmp{$nKey}[1] = $g_arrSrcDbHandle[$nA]->quote($hashTmp{$nKey}[1]);
				
				$hashTmp2{$nDstSrvIndex}{$nKey} = $hashTmp{$nKey};
			}
			print "\t\tRedistribute Calc End:".&GetLocalStringTime()."=========\n";
			my $strQueryHead = "replace into $g_strDbName.t_rc_online_peer_list_temp_$nB values";
			my $strQueryTail = ";";
			$strQuery = "";
			foreach my $nDstSrvIndex (sort keys %hashTmp2)
			{	
				print "DstSrv:$nDstSrvIndex==============\n";
				my $nCount = 0 ;
				for my $nKey (keys %{$hashTmp2{$nDstSrvIndex}})
				{
					if($nCount == 0)
					{
						$strQuery .= "($hashTmp{$nKey}[0], $hashTmp{$nKey}[1], $hashTmp{$nKey}[2], $hashTmp{$nKey}[3], $hashTmp{$nKey}[4], $hashTmp{$nKey}[5], $hashTmp{$nKey}[6], $hashTmp{$nKey}[7])";
					}
					else
					{
						$strQuery .= ",($hashTmp{$nKey}[0], $hashTmp{$nKey}[1], $hashTmp{$nKey}[2], $hashTmp{$nKey}[3], $hashTmp{$nKey}[4], $hashTmp{$nKey}[5], $hashTmp{$nKey}[6], $hashTmp{$nKey}[7])";
					}
					$nCount ++;
					if ($nCount >= 200)
					{
							my $sthInsert = &Execute($g_arrDstDbHandle[$nDstSrvIndex], $strQueryHead.$strQuery.$strQueryTail);
							if (!defined($sthInsert))
							{
								print "insert into DsbSrv[$nDstSrvIndex] fail\n";
								sleep(1);
							}
							$nCount = 0;
							$strQuery = "";	
					}
				}
				if ($nCount > 0)
				{
					my $sthInsert = &Execute($g_arrDstDbHandle[$nDstSrvIndex], $strQueryHead.$strQuery.$strQueryTail);
					if (!defined($sthInsert))
					{
						print "insert into DsbSrv[$nDstSrvIndex] fail\n";
						sleep(1);
					}
					$nCount = 0;
					$strQuery = "";	
				}
			}
			print "\tReDistribute $nA:$nB End:".&GetLocalStringTime()."=========\n";
			%hashTmp = ();
			%hashTmp2 = ()
		}
	}
	print "End:".&GetLocalStringTime()."=========\n";
}

sub ConnectSrv()
{
	@g_arrSrcDbHandle = ();
	print "Connect Src Db===================\n";
	for (my $nA = 0; $nA < $g_nSrcDbNum; ++ $nA)
	{
		my $hMysql = &Connect($g_strDbName, $g_arrHashSrcDb[$nA], $g_strUser, $g_strPasswd);
		if (!defined($hMysql))
		{
			print "Connect SrcSrv $g_arrHashSrcDb[$nA] fail\n";
			return -1;
		}
		$g_arrSrcDbHandle[$nA] = $hMysql;
		print "Connect SrcSrv $g_arrHashSrcDb[$nA] succ\n";
	}
	print "Connect Dst Db===================\n";
	for (my $nA = 0; $nA < $g_nDstDbNum; ++ $nA)
	{
		my $hMysql = &Connect($g_strDbName, $g_arrHashDstDb[$nA], $g_strUser, $g_strPasswd);
		if (!defined($hMysql))
		{
			print "Connect DstSrv $g_arrHashDstDb[$nA] fail\n";
			return -1;
		}
		$g_arrDstDbHandle[$nA] = $hMysql;
		print "Connect DstSrv $g_arrHashDstDb[$nA] succ\n";
	}
	return 0;
}

sub LoadSrcSrv()
{
	my $nRet = open(SRCDBCONF, "<", "$g_strSrcDbConf");
	if (!defined($nRet))
	{
		print "LoadSrcSrv: open file $g_strSrcDbConf fail\n";
		return -1;
	}
	my $nIndex = 0;
	@g_arrHashSrcDb = ();
	print "SrcDb=============\n";
	while(my $strLine = <SRCDBCONF>)
	{
		if (!($strLine =~ s/\r\n$//))
		{
			$strLine =~ s/\n$//;
		}
		if((substr($strLine, 0, 1) eq '#') || $strLine eq '')
		{
			next;
		}
		$g_arrHashSrcDb[$nIndex] = $strLine;
		print "$nIndex: $g_arrHashSrcDb[$nIndex]\n";
		$nIndex ++;
	}
	close(SRCDBCONF);
	if (@g_arrHashSrcDb != $g_nSrcDbNum)
	{
		@g_arrHashSrcDb = ();
		print "SrcConf not match srv num\n";
		return -1;
	}
	return 0;
}

sub LoadDstSrv()
{
	my $nRet = open(DSTDBCONF, "<", "$g_strDstDbConf");
	if (!defined($nRet))
	{
		print "LoadDstSrv: open file $g_strDstDbConf fail\n";
		return -1;
	}
	
	my $nIndex = 0;
	@g_arrHashDstDb = ();
	print "DstDb=============\n";
	while(my $strLine = <DSTDBCONF>)
	{
		if (!($strLine =~ s/\r\n$//))
		{
			$strLine =~ s/\n$//;
		}
		if((substr($strLine, 0, 1) eq '#') || $strLine eq '')
		{
			next;
		}
		$g_arrHashDstDb[$nIndex] = $strLine;
		print "$nIndex: $g_arrHashDstDb[$nIndex]\n";
		$nIndex ++;
	}
	close(DSTDBCONF);
	if (@g_arrHashDstDb != $g_nDstDbNum)
	{
		@g_arrHashSrcDb = ();
		print "DstConf not match srv num\n";
		return -1;
	}
	return 0;
}


sub Connect()
{
	#my ($strDbName, $strIp, $strUser, $strPasswd) = @_;
	my $dbhMysql;
	eval
	{
		$dbhMysql = DBI->connect("DBI:mysql:database=$_[0];$_[1]",$_[2],$_[3], {'RaiseError' => 1, 'PrintError' => 0});
	};
	if($@)
	{
		print "MySql::Connect : Connect Mysql Fail :  $DBI::errstr\n";
		return undef;
	}
	return $dbhMysql;
}

sub Disconnect()
{
	#my ($dbhMysql) = @_;
	eval
	{
		$_[0]->disconnect();	
	};
	if($@)
	{
		print "MySql::Disconnect : Close Mysql Fail :  $DBI::errstr\n";
		undef;
	}
	else 
	{
		return 0;
	}	
}

sub Escape()
{
	#my($dbhMysql, $strString) = @_;
	my $strEscaped;
	eval
	{
		$strEscaped = $_[0]->quote($_[1]);
	};
	if($@)
	{
		print "MySql::Escape : Error : $_[1] :  $DBI::errstr\n";
		return undef;
	}
	return $strEscaped;
}

sub Execute()
{
	#my($dbhMysql, $strSql) = @_;
	my $sthMysql;
	eval
	{
		$sthMysql = $_[0]->prepare($_[1]);
		$sthMysql->execute();
	};
	if($@)
	{
		print "MySql::Execute : Error : $_[1] :  $DBI::errstr\n";
		return undef;
	}
	return $sthMysql;
}

sub FetchrowArray()
{
	#my($sthMysql) = @_;
	my @arrFetch  = ();
	eval
	{
		@arrFetch = $_[0]->fetchrow_array();
	};
	if($@)
	{
		print "MySql::FetchrowArray : Error :  $DBI::errstr\n";
		return ();
	}
	return @arrFetch;
}

sub FetchrowArrayRef()
{
	#my($sthMysql) = @_;
	my $refArrFetch;
	eval
	{
		$refArrFetch = $_[0]->fetchrow_arrayref();
	};
	if($@)
	{
		print "MySql::FetchrowArrayRef : Error : $DBI::errstr\n";
		return undef;
	}
	return $refArrFetch;
}

sub FetchrowHashRef()
{
	#my($sthMysql) = @_;
	my $refHashFetch;
	eval
	{
		$refHashFetch =  $_[0]->fetchrow_hashref();
	};
	if($@)
	{
		print "MySql::FetchrowHashRef : Error : $DBI::errstr\n";
		return undef;
	}
	return $refHashFetch;
}

sub CreateDatabase()
{
	#my($dbhMysql, $strDbName) = @_;
	my $strMysql = "create database if not exists ".$_[1];
	return &MysqlExecute($_[0], $strMysql);
}

sub CreateTableLike()
{	
	#my ($dbhMysql, $strDbTable, $strDbTableLike, ) = @_;
	my $strMySql = "create table if not exists $_[1] like $_[2]";
	return &MysqlExecute($_[0], $strMySql);
}

sub ShortELFHash()
{
	my ($strString) = @_;
	my $nH = 0;
	my $nG = 0;
	my $chChar = '';
	my $nChar = 0;
	my $nLength = length($strString);

	for(my $nA = 0; $nA < $nLength; ++ $nA)
	{
		$chChar = substr($strString, $nA, 1);
		$nChar = unpack('c', $chChar);
		$nH = ($nH << 4) + $nChar;
		$nG = $nH & 0xF0000000;
		if($nG)
		{
			$nH ^= $nG >> 24;
		}
		$nH &= ~$nG;
	}
	return $nH;
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
