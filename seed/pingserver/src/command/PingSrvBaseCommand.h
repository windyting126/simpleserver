/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: PingSrvBaseCommand.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-08-04
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-08-04			1.0			
* =======================================================================
*/
#ifndef _PINGSRVBASECOMMAND_H_7FD12EB4_8F17_4b06_8835_5C97D0908C3B_
#define _PINGSRVBASECOMMAND_H_7FD12EB4_8F17_4b06_8835_5C97D0908C3B_


#include <vector>
#include "basecommand.h"
#include "framecommon/framecommon.h"
#include "PingServerDef.h"
#include "PingServer.h"

using namespace std;
using namespace MYFRAMECOMMON;

struct TPingSrvBaseCommand
{
	int m_nProtocolVersion;		//协议版本
	char m_nCmd;
	TPingSrvBaseCommand()
	{
		m_nProtocolVersion = 256;
		m_nCmd = 0;
	}
	int Encode(CNewBufMgr &bufMgr)
	{
		int ret = 0;
		ret += bufMgr.PutInt(m_nProtocolVersion);
		ret += bufMgr.PutChar(m_nCmd);
		return ret;
	}
	int Decode(CNewBufMgr &bufMgr)
	{
		int ret = 0;
		ret += bufMgr.GetInt(m_nProtocolVersion);
		ret += bufMgr.GetChar(m_nCmd);
		return ret;
	}
	void Printf()
	{
		printf("version is %d\n", m_nProtocolVersion);
	}
};

template <class T1, class T2 = CDefaultRes> 
class CPingSrvBaseCommand : public CCommand
{
public:
	static int GetCommandId(char *pBuf, int nLen) //zz
	{
		if(nLen < PINGSERVER_MSG_HEAD_LEN) 
		{
			WriteRunInfo::WriteLog("The cmd buf len is less than %d, "
				"too short", PINGSERVER_MSG_HEAD_LEN);
			return INVALID_CMD_ID;
		}

		unsigned char uchCmdId;
		memcpy(&uchCmdId, pBuf + PINGSERVER_CMDINFO_OFF, sizeof(uchCmdId));
		return uchCmdId;		//ntohs(uchCmdId);//根据情况看是否需要转换字节序
	}
	CPingSrvBaseCommand(char *pBuf, int nLen, int nClientIp, short nClientPort, int nEntityType, int nEntityId)
		: m_inBuf(NULL, 0, 1, 0), m_outBuf(NULL, 0, 0, 0)
	{
		m_pServerConf = &((CPingServer *)CMyserver::GetInstance())->GetConf();
		m_pHashTable = ((CPingServer *)CMyserver::GetInstance())->GetPeerHashTable();
		m_outHead.m_nProtocolVersion = PINGSERVER_VERSION;
		m_inBuf.AttachBuf(pBuf, nLen);
		m_nIfDecode=0;
		m_nResult = -1;

		m_nEntityType = nEntityType;
		m_nEntityId = nEntityId;

		m_nClientIp = nClientIp;
		m_nClientPort = nClientPort;
	}
	virtual ~CPingSrvBaseCommand(){}

	int Process2()
	{
		return ProcessCmd();
	}

	int Process(int &nResult)
	{
		//解码
		nResult = 0;
		int nRet = Decode();
		if (nRet == 0)
		{
			WRITE_TRACE("Decode Succeed");
			nResult =  ProcessCmd()	;
			if (nResult == 0)
			{
				WRITE_TRACE("Process Succeed");
			}
			else
			{
				WRITE_TRACE("Process Failed");
			}
			nRet = nResult;
		}
		else
		{
			WRITE_TRACE("Decode Failed");
		}

		if (nRet == 0 && m_nIfRes == 1) 
		{   
			GenResHead(); //生成返回的头部
			nRet = Encode();
			if (nRet == 0)
			{
				WRITE_TRACE("Encode Succeed");
			}
			else
			{
				WRITE_TRACE("Encode Failed");
			}			
		}			
		return nRet;
	}

	T1 &GetIn()
	{
		return m_in;
	}

	char *GetResBuf(int &nLen)
	{
		nLen = m_outBuf.GetRealBufLen();
		return (char*)m_outBuf.GetBuf();
	}

	void DisAttachBuf()
	{
		m_outBuf.DisAttachBuf();
	}

	short GetMsgType()
	{
		return m_inHead.m_nCmd;
	}
protected:
	void GenResHead()
	{
		m_outHead.m_nProtocolVersion = htonl(PINGSERVER_VERSION);
	}
	int Encode()
	{
		int iPacketLength = m_out.GetSize() + sizeof(int) + sizeof(char) + sizeof(char);
		char * pEncodeBuf = new char[iPacketLength];
		m_outBuf.AttachBuf(pEncodeBuf , iPacketLength);

		int ret=0;
		ret+=m_outBuf.PutInt(m_outHead.m_nProtocolVersion);
		ret+=m_outBuf.PutChar(m_outHead.m_nCmd);
		//ret+=m_outBuf.PutChar(m_nResult);

		ret += m_out.Encode(m_outBuf);
		return ret;
	}
	int Decode()
	{
		int ret=0;
		if (m_nIfDecode == 0)
		{
			ret+=m_inBuf.GetInt((int&)m_inHead.m_nProtocolVersion);
			m_inHead.m_nProtocolVersion = ntohl(m_inHead.m_nProtocolVersion);
			ret+=m_inBuf.GetChar((char&)m_inHead.m_nCmd);

			ret+=m_in.Decode(m_inBuf);	
			m_nIfDecode = 1;
		}            
		return ret;
	}

	void TransMsg(char *pClientCommandPacketBuf, unsigned int uClientCommandPacketLen, int iServerIndex, int nServerType)
	{
		CTmpBufMgr GenPacketHelper;
		int nTmpLen = sizeof(SERVERINNERCOMMANDPACKET) + uClientCommandPacketLen;
		char *pTmpBuf;
		pTmpBuf = new char[nTmpLen];
		GenPacketHelper.AttachBuf(pTmpBuf, nTmpLen);

		SERVERINNERCOMMANDPACKET serverInnerCommandPacket;
		serverInnerCommandPacket.ucDestMachineType = nServerType;
		serverInnerCommandPacket.usDestMachineIndex = iServerIndex;

		serverInnerCommandPacket.serverCommandPacket.usMsgId = (unsigned short)htons(ONLINE_SYN);
		serverInnerCommandPacket.serverCommandPacket.iMsgLen = (int)htonl(uClientCommandPacketLen);
		GenPacketHelper.Put(serverInnerCommandPacket);

		GenPacketHelper.PutCharBuf(pClientCommandPacketBuf, uClientCommandPacketLen);

		TMsg tMsg;
		tMsg.destEntityType = ((iServerIndex == -1 && nServerType == -1) ? UDP_MSEND_ENTITY_TYPE : LONGCONNECTEDENTITY);
		tMsg.destEntityId = 0;
		tMsg.srcEntityId = m_nEntityId;
		tMsg.srcEntityType= m_nEntityType;
		tMsg.srcTaskId = 0;
		tMsg.taskId = 0;
		tMsg.msgType = SYN_MSGTYPE;

		CMsgBody * pMsgBody = new CClientMsgBody(GenPacketHelper.GetBuf(), GenPacketHelper.GetRealBufLen());
		GenPacketHelper.DisAttachBuf();
		CMsg *pMsg = new CMsg(tMsg, pMsgBody);
		CMyserver::GetInstance()->PutMsgToThread(pMsg);
	}
protected:
	virtual int ProcessCmd() = 0;
protected:
	int m_nIfDecode;
	TPingSrvBaseCommand m_inHead;	
	TPingSrvBaseCommand m_outHead;
	CBufMgr m_inBuf;
	CTmpBufMgr m_outBuf;
	int m_nResult;
	T1 m_in;
	T2 m_out;

	int m_nEntityType;
	int m_nEntityId;

	CPingServerConf *m_pServerConf;
	CPingPeerHashTable *m_pHashTable;
	int m_nClientIp;
	short m_nClientPort;

	//CPeerHashTable * m_pPeerHashTable;
};



#endif

