-- MySQL dump 10.11
--
-- Host: localhost    Database: resource_peer
-- ------------------------------------------------------
-- Server version	5.0.45

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

drop database if exists presource_peer_new;
create database presource_peer_new;
use presource_peer_new;

--
-- Table structure for table `t_online_peer_list`
--

DROP TABLE IF EXISTS `t_online_peer_list`;
CREATE TABLE `t_online_peer_list` (
  `fpeerid` varchar(40) collate latin1_bin NOT NULL default ''
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_bin;

--
-- Dumping data for table `t_online_peer_list`
--

LOCK TABLES `t_online_peer_list` WRITE;
/*!40000 ALTER TABLE `t_online_peer_list` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_online_peer_list` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_online_peer_list_temp`
--

DROP TABLE IF EXISTS `t_online_peer_list_temp`;
CREATE TABLE `t_online_peer_list_temp` (
  `fpeerid` varchar(40) collate latin1_bin NOT NULL,
  PRIMARY KEY  (`fpeerid`)
) ENGINE=MEMORY DEFAULT CHARSET=latin1 COLLATE=latin1_bin;

--
-- Dumping data for table `t_online_peer_list_temp`
--

LOCK TABLES `t_online_peer_list_temp` WRITE;
/*!40000 ALTER TABLE `t_online_peer_list_temp` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_online_peer_list_temp` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_rc_online_peer_list`
--

DROP TABLE IF EXISTS `t_rc_online_peer_list`;
CREATE TABLE `t_rc_online_peer_list` (
  `fcid` varchar(40) collate latin1_bin NOT NULL default '',
  `fpeerid` varchar(50) collate latin1_bin NOT NULL default '',
  `fpublic_ip` int(10) unsigned NOT NULL default '0',
  `ffilesize` int(16) NOT NULL default '0',
  `fflag` tinyint(4) NOT NULL default '0',
  `fcapability` int(11) NOT NULL default '0',
  KEY `t_rc_online_peer_list_fcid` (`fcid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_bin;

--
-- Dumping data for table `t_rc_online_peer_list`
--

LOCK TABLES `t_rc_online_peer_list` WRITE;
/*!40000 ALTER TABLE `t_rc_online_peer_list` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_rc_online_peer_list` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_rc_online_peer_list_temp`
--

DROP TABLE IF EXISTS `t_rc_online_peer_list_temp`;
CREATE TABLE `t_rc_online_peer_list_temp` (
  `fcid` varchar(40) collate latin1_bin NOT NULL default '',
  `fpeerid` varchar(50) collate latin1_bin NOT NULL default '',
  `fpublic_ip` int(10) unsigned NOT NULL default '0',
  `ffilesize` int(16) NOT NULL default '0',
  `fflag` tinyint(4) NOT NULL default '0',
  `fcapability` int(11) NOT NULL default '0',
  `ftime` int(11) unsigned NOT NULL default '0',
  `downpercent` int(3) unsigned NOT NULL default '255',
  PRIMARY KEY  (`fcid`,`fpeerid`),
  KEY `t_rc_online_peer_list_temp_fcid` (`fcid`),
  KEY `downpercent` (`downpercent`)
) ENGINE=MEMORY DEFAULT CHARSET=latin1 COLLATE=latin1_bin;

--
-- Dumping data for table `t_rc_online_peer_list_temp`
--

LOCK TABLES `t_rc_online_peer_list_temp` WRITE;
/*!40000 ALTER TABLE `t_rc_online_peer_list_temp` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_rc_online_peer_list_temp` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_rc_online_peer_list_temp_temp`
--

DROP TABLE IF EXISTS `t_rc_online_peer_list_temp_temp`;
CREATE TABLE `t_rc_online_peer_list_temp_temp` (
  `fcid` varchar(40) collate latin1_bin NOT NULL default '',
  `fpeerid` varchar(50) collate latin1_bin NOT NULL default '',
  `fpublic_ip` int(10) unsigned NOT NULL default '0',
  `ffilesize` int(16) NOT NULL default '0',
  `fflag` tinyint(4) NOT NULL default '0',
  `fcapability` int(11) NOT NULL default '0',
  `ftime` int(11) unsigned NOT NULL default '0',
  `downpercent` int(3) unsigned NOT NULL default '255',
  PRIMARY KEY  (`fcid`,`fpeerid`),
  KEY `t_rc_online_peer_list_temp_temp_fcid` (`fcid`),
  KEY `downpercent` (`downpercent`)
) ENGINE=MEMORY DEFAULT CHARSET=latin1 COLLATE=latin1_bin;

--
-- Dumping data for table `t_rc_online_peer_list_temp_temp`
--

LOCK TABLES `t_rc_online_peer_list_temp_temp` WRITE;
/*!40000 ALTER TABLE `t_rc_online_peer_list_temp_temp` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_rc_online_peer_list_temp_temp` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_rc_peer_list`
--

DROP TABLE IF EXISTS `t_rc_peer_list`;
CREATE TABLE `t_rc_peer_list` (
  `fcid` varchar(40) collate latin1_bin NOT NULL default '',
  `fpeerid` varchar(50) collate latin1_bin NOT NULL default '',
  `fpublic_ip` int(10) unsigned NOT NULL default '0',
  `ffilesize` int(16) NOT NULL default '0',
  `fflag` tinyint(4) NOT NULL default '0',
  `fcapability` int(11) NOT NULL default '0',
  `ftime` int(11) unsigned NOT NULL default '0',
  `downpercent` int(3) unsigned NOT NULL default '255',
  PRIMARY KEY  (`fcid`,`fpeerid`),
  KEY `t_rc_peer_list_ftime` (`ftime`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_bin;

--
-- Dumping data for table `t_rc_peer_list`
--

LOCK TABLES `t_rc_peer_list` WRITE;
/*!40000 ALTER TABLE `t_rc_peer_list` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_rc_peer_list` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_rc_peer_list_temp`
--

DROP TABLE IF EXISTS `t_rc_peer_list_temp`;
CREATE TABLE `t_rc_peer_list_temp` (
  `fcid` varchar(40) collate latin1_bin NOT NULL default '',
  `fpeerid` varchar(50) collate latin1_bin NOT NULL default '',
  `fpublic_ip` int(10) unsigned NOT NULL default '0',
  `ffilesize` int(16) NOT NULL default '0',
  `fflag` tinyint(4) NOT NULL default '0',
  `fcapability` int(11) NOT NULL default '0',
  `ftime` int(11) unsigned NOT NULL default '0',
  `downpercent` int(3) unsigned NOT NULL default '255',
  PRIMARY KEY  (`fcid`,`fpeerid`),
  KEY `t_rc_peer_list_temp_ftime` (`ftime`)
) ENGINE=MEMORY DEFAULT CHARSET=latin1 COLLATE=latin1_bin;

--
-- Dumping data for table `t_rc_peer_list_temp`
--

LOCK TABLES `t_rc_peer_list_temp` WRITE;
/*!40000 ALTER TABLE `t_rc_peer_list_temp` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_rc_peer_list_temp` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2009-08-28 10:52:20
