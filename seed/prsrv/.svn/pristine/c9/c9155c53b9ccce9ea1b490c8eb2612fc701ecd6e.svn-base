// test_rserver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TcpSocketHelper.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <string>
using namespace std;

const unsigned int PEER_LENGTH=16; //peeridlen
const unsigned int CID_LEN=20;    //cidlen

//#define _SIM_CS_

void GetRandomString(int nLeng, string &result)
{
	static char ch_table[] = {"01234567890!@#$%^&*()ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"};
	int cn_table_size = strlen(ch_table);

	for (int i=0; i<nLeng; ++i)
	{
		char ch = ch_table[rand()%cn_table_size];
		result.append(&ch,sizeof(char));
	}
}

/*
---HEADER---
int32 version;
int32 seq;
int32 length;
int16 id; 

*/
BYTE *PacketBuilder(unsigned int version, unsigned int seq, unsigned short id, 
					const unsigned char *data, unsigned int data_size, 
					unsigned int &buf_size)
{
	buf_size = sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned short)
		+ sizeof(unsigned int) + data_size;

	BYTE *buf = new BYTE [buf_size];
	if (!buf)
	{
		return NULL;
	}

	unsigned int offset = 0;

	
	unsigned int ver_ = htonl(version);
	memcpy(buf+offset, &ver_,sizeof(unsigned int));
	offset += sizeof(unsigned int);

	unsigned int seq_ = htonl(seq);
	memcpy(buf+offset, &seq_,sizeof(unsigned int));
	offset += sizeof(unsigned int);

	unsigned int leng_ = htonl(data_size+sizeof(unsigned short));
	memcpy(buf+offset, &leng_, sizeof(unsigned int));
	offset += sizeof(unsigned int);

	unsigned short id_ = htons(id);
	memcpy(buf+offset, &id_, sizeof(unsigned short));
	offset += sizeof(unsigned short);

	memcpy(buf+offset, data, data_size);

	return buf;
}

/*
---HEADER---
int32 version;
int32 seq;
int32 length;
int16 id;
int8 result;
*/
BYTE *PacketParser(const BYTE *buf, unsigned int buf_size, 
				   unsigned int &version, unsigned int &seq, unsigned short &id,
				   unsigned char &result, unsigned int &attached_data_size)
{
	attached_data_size = 0;

	unsigned int offset = 0;

	unsigned int _version = 0;
	memcpy(&_version, buf+offset, sizeof(unsigned int));
	version = ntohl(_version);
	offset += sizeof(unsigned int);

	if (offset > buf_size)
	{ //oops!
		return NULL;
	}
	unsigned int _seq = 0;
	memcpy(&_seq, buf+offset, sizeof(unsigned int));
	seq = ntohl(_seq);
	offset += sizeof(unsigned int);

	if (offset > buf_size)
	{ //oops!
		return NULL;
	}
	unsigned int _length = 0, length = 0;
	memcpy(&_length, buf+offset, sizeof(unsigned int));
	length = ntohl(_length);
	offset += sizeof(unsigned int);

	if (offset > buf_size)
	{ //oops!
		return NULL;
	}
	unsigned short _id = 0;
	memcpy(&_id, buf+offset, sizeof(unsigned short));
	id = ntohs(_id);
	offset += sizeof(unsigned short);

	if (offset > buf_size)
	{ //oops!
		return NULL;
	}
	unsigned char _result = 0;
	memcpy(&_result, buf+offset, sizeof(unsigned char));
	result = _result;
	offset += sizeof(unsigned char);

	if (length > sizeof(unsigned short)+sizeof(unsigned char))
	{ //more data following...
		attached_data_size = length - sizeof(unsigned short) - sizeof(unsigned char);
		return (BYTE*)(buf+offset);
	}

	//no more fillowing data
	
	return NULL;
}

#pragma pack( push, 1 )

typedef struct tagSERVERCOMMANDPACKET {
		unsigned short usMsgId;
		unsigned int     uSrcClientIp;
		unsigned short  usSrcClientPort;
		unsigned long long ullSrcTaskId;
		int iSrcProcessId;
		unsigned long long ullDestTaskId;
		int iDestProcessId;
		int iMsgLen;
} SERVERCOMMANDPACKET;

#pragma pack( pop )


BYTE *build_clt_logon_packet(unsigned int &nsize)
{
	printf("Send logon cmd...\r\n");

	char buf[512] = {0};
	int offset = 0;

	string peerid;
	GetRandomString(PEER_LENGTH, peerid);

	int peedid_size = htonl(PEER_LENGTH);
	memcpy(buf+offset, &peedid_size, sizeof(int));
	offset += sizeof(int);
	
	memcpy(buf+offset, peerid.c_str(), PEER_LENGTH);
	offset += PEER_LENGTH;
		
	return PacketBuilder(0, 0, 1000, (unsigned char*)buf, offset, nsize);
}

BYTE *build_clt_report_resource_packet(unsigned int &nsize)
{
	printf("Send report_rc cmd...\r\n");

	char *rc_buf = new char [1024];

	int offset = 0;

	string pid;
	GetRandomString(PEER_LENGTH,pid);
	
	int pid_size = htonl(PEER_LENGTH);
	memcpy(rc_buf+offset, &pid_size, sizeof(int));
	offset += sizeof(int);

	memcpy(rc_buf+offset, pid.c_str(), PEER_LENGTH);
	offset += PEER_LENGTH;

	int cap = htonl(30);
	memcpy(rc_buf+offset, &cap, sizeof(int));
	offset += sizeof(int);


	int rc_cnt = htonl(1);
	memcpy(rc_buf+offset, &rc_cnt, sizeof(int));
	offset += sizeof(int);

	string cid;
	GetRandomString(CID_LEN,cid);

	int cid_size = htonl(CID_LEN);
	memcpy(rc_buf+offset, &cid_size, sizeof(int));
	offset += sizeof(int);

	memcpy(rc_buf+offset, cid.c_str(), CID_LEN);
	offset += CID_LEN;

	int down_scale = htonl(35);
	memcpy(rc_buf+offset, &down_scale, sizeof(int));
	offset += sizeof(int);
	

	BYTE *presult = PacketBuilder(0, 1, 1002, (unsigned char*)rc_buf, offset, nsize);

	delete [] rc_buf;

	return presult;
}

BYTE *build_clt_report_update_packet(unsigned int &nsize)
{
	printf("Send insert_rc cmd...\r\n");

	char *rc_buf = new char [1024];

	int offset = 0;

	int ncid = rand();
	string cid;
	GetRandomString(CID_LEN,cid);
	
	int cid_size = htonl(CID_LEN);
	memcpy(rc_buf+offset, &cid_size, sizeof(int));
	offset += sizeof(int);
	memcpy(rc_buf+offset, cid.c_str(), CID_LEN);
	offset += CID_LEN;

	string pid;
	GetRandomString(PEER_LENGTH,pid);
	int pid_size = htonl(PEER_LENGTH);
	memcpy(rc_buf+offset, &pid_size, sizeof(int));
	offset += sizeof(int);
	memcpy(rc_buf+offset, pid.c_str(), PEER_LENGTH);
	offset += PEER_LENGTH;
		
	int down_scale = htonl(rand()%100);
	memcpy(rc_buf+offset, &down_scale, sizeof(int));
	offset += sizeof(int);

	int cap = htonl(rand()%10000);
	memcpy(rc_buf+offset, &cap, sizeof(int));
	offset += sizeof(int);


	BYTE *presult = PacketBuilder(0, 1, 1004, (unsigned char*)rc_buf, offset, nsize);

	delete [] rc_buf;

	return presult;
}

BYTE *build_clt_report_delete_packet(unsigned int &nsize, char *cid, char *pid)
{
	printf("Send delete_rc cmd...\r\n");

	char *rc_buf = new char [1024];

	int offset = 0;
	
	//char cid[] = {"5"};
	int cid_size = htonl(strlen(cid));
	memcpy(rc_buf+offset, &cid_size, sizeof(int));
	offset += sizeof(int);
	memcpy(rc_buf+offset, cid, strlen(cid));
	offset += (strlen(cid));


	int pid_size = htonl(strlen(pid));
	memcpy(rc_buf+offset, &pid_size, sizeof(int));
	offset += sizeof(int);
	memcpy(rc_buf+offset, pid, strlen(pid));
	offset += (strlen(pid));

	
	BYTE *presult = PacketBuilder(0, 1, 1006, (unsigned char*)rc_buf, offset, nsize);

	delete [] rc_buf;

	return presult;
}

bool parse_srv_logon_response_packet(const char *buf, unsigned int buf_size, 
									 unsigned char &result, char &report_rc_flag)
{
	unsigned int version=0, seq=0, attached_data_size=0;
	unsigned short id=0;
	
	BYTE *attached_data = PacketParser((const BYTE*)buf, buf_size, version, seq, id, result, attached_data_size);

	if (id != 1001)
	{ //oops!
		return false;
	}

	if (attached_data_size != sizeof(char))
	{ //oops!
		return false;
	}

	memcpy(&report_rc_flag, (char*)attached_data, sizeof(char));
    return true; 
}

bool parse_srv_report_rc_response_packet(const char *buf, unsigned int buf_size, 
									 unsigned char &result)
{
	unsigned int version=0, seq=0, attached_data_size=0;
	unsigned short id=0;

	BYTE *attached_data = PacketParser((const BYTE*)buf, buf_size, version, seq, id, result, attached_data_size);

	if (id != 1003)
	{ //oops!
		return false;
	}

	if (attached_data_size != 0)
	{ //oops!
		return false;
	}

	return true; 
}

bool parse_srv_report_update_response_packet(const char *buf, unsigned int buf_size, 
										 unsigned char &result)
{
	unsigned int version=0, seq=0, attached_data_size=0;
	unsigned short id=0;

	BYTE *attached_data = PacketParser((const BYTE*)buf, buf_size, version, seq, id, result, attached_data_size);

	if (id != 1005)
	{ //oops!
		return false;
	}

	if (attached_data_size != 0)
	{ //oops!
		return false;
	}

	return true; 
}

bool parse_srv_report_delete_response_packet(const char *buf, unsigned int buf_size, 
											 unsigned char &result)
{
	unsigned int version=0, seq=0, attached_data_size=0;
	unsigned short id=0;

	BYTE *attached_data = PacketParser((const BYTE*)buf, buf_size, version, seq, id, result, attached_data_size);

	if (id != 1007)
	{ //oops!
		return false;
	}

	if (attached_data_size != 0)
	{ //oops!
		return false;
	}

	return true; 
}

void usage()
{
	printf("USAGE: test_rserver [option] [args]\r\n");
	printf("option can be any of the following:\r\n");
	printf(" 0 - send logon cmd (default)\r\n");
	printf(" 1 - send report_rc cmd\r\n");
	printf(" 2 - send insert_rc cmd\r\n");
	printf(" 3 - send delete_rc cmd, cid and pid should be specified.\r\n\r\n");
}


int _tmain(int argc, _TCHAR* argv[])
{
	usage();

	char cid[512] = {"1"};
	char pid[512] = {"1"};

	int nCmd = 0; // 0 - logon; 1 - report rc; 2 - insert rc; 3 - delete rc , argv[2] = cid, argv[3] = pid

	if (argc >= 2)
	{
		nCmd = atoi(argv[1]);
	}

	if (nCmd == 3)
	{
		if (argc == 4)
		{
			strcpy(cid, argv[2]);
			strcpy(pid, argv[3]);
    	}
	}

	srand( (unsigned)time( NULL ) );

	CTcpClientSocketHelper tcpclt;

	char szSrvIp[100];
	DWORD dwPort = 0;

#ifdef _SIM_CS_
	strcpy(szSrvIp, "192.168.50.15");
	dwPort = 8099;
#else
	strcpy(szSrvIp, "192.168.50.15");
	dwPort = 8011;
#endif

	printf("Connecting to %s:%d ... ", szSrvIp, dwPort);
	if (0 == tcpclt.Init(szSrvIp,dwPort))
	{
		printf("succeed!\r\n");
	}
	else
	{
		printf("failed!!!\r\n");
		printf("Press any key to cxit.");
		getchar();
		return -1;
	}

	unsigned int buf_size = 0;

	BYTE *buf = 0;
	
	switch (nCmd)
	{
	case 0:
		buf = build_clt_logon_packet(buf_size);
		break;
	case 1:
		buf = build_clt_report_resource_packet(buf_size);
		break;
	case 2:
		buf = build_clt_report_update_packet(buf_size);
		break;
	case 3:
		buf = build_clt_report_delete_packet(buf_size, cid, pid);
		break;
	default:;
	}
	

//********************************************************
#ifdef _SIM_CS_
	SERVERCOMMANDPACKET srvCommHeader = {0};

	srvCommHeader.iMsgLen = htonl(buf_size);
	srvCommHeader.usMsgId = htons(2000);

	BYTE *pNewBuf = new BYTE[sizeof(SERVERCOMMANDPACKET)+buf_size];
	memcpy(pNewBuf, &srvCommHeader, sizeof(SERVERCOMMANDPACKET));
	memcpy(pNewBuf+sizeof(SERVERCOMMANDPACKET), buf, buf_size);

	DWORD dwNewBufSize = sizeof(SERVERCOMMANDPACKET)+buf_size;
#endif
//********************************************************

	BYTE *pBufToSend = NULL;
	DWORD dwSizeToSend = 0;

#ifdef _SIM_CS_ 
	pBufToSend = pNewBuf;
	dwSizeToSend = dwNewBufSize;
#else
	pBufToSend = buf;
	dwSizeToSend = buf_size;
#endif



	if (tcpclt.SendData(pBufToSend, (DWORD*)&dwSizeToSend))
	{
		printf("send data succeed!\r\n");
	}
	else
	{
		printf("send data failed!!!\r\n");
	}

	delete [] buf;

#ifdef _SIM_CS_
//********************************************************
	delete [] pNewBuf;
//********************************************************
#endif 

		printf("About to receive response packet ...\r\n");

	buf = new BYTE[500*1024]; //500k
	DWORD  dwBufSize= 500*1024;
	BOOL bRet = tcpclt.ReceiveData(buf, &dwBufSize);

	unsigned char result = 0;
	unsigned int srv_cmd_header_size = 0;
	
#ifdef _SIM_CS_
//********************************************************
	srv_cmd_header_size = sizeof(SERVERCOMMANDPACKET);
//********************************************************
#endif

	dwBufSize -= srv_cmd_header_size;

	char report_rc_flag = 0;

	bool ret = false;

	switch (nCmd)
	{
	case 0:
		ret = parse_srv_logon_response_packet((const char*)buf+srv_cmd_header_size, dwBufSize, result, report_rc_flag);
		break;
	case 1:
		ret = parse_srv_report_rc_response_packet((const char*)buf+srv_cmd_header_size, dwBufSize, result);
		break;
	case 2:
		ret = parse_srv_report_update_response_packet((const char*)buf+srv_cmd_header_size, dwBufSize, result);
		break;
	case 3:
		ret = parse_srv_report_delete_response_packet((const char*)buf+srv_cmd_header_size, dwBufSize, result);
		break;
	default:;
	}
	
	if (ret)
	{
		printf("receive response succeed with result = %d\r\n", result);
	}
	else
	{
		printf("receive response failed!!!\r\n");
	}

	delete [] buf;

	//printf("Press any key to cxit.");
	//getchar();

	return 0;
}

