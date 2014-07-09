/*
* =======================================================================
* Copyright (c) 2011-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: seedbasecommand.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2011-03-25
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2011-03-25			1.0			
* =======================================================================
*/
#ifndef __SEEDBECOMMAND_H_1D229D52_D554_47F8_83A7_57F6F9CB014A_
#define __SEEDBECOMMAND_H_1D229D52_D554_47F8_83A7_57F6F9CB014A_
#include "basecommand.h"
#include "rsrvdef.h"
#include "mycommmsg.h"

struct TSeedBaseCmd
{
	int m_nProtocolVersion;		//协议版本
	int m_nSeqence;				//消息序列号
	int m_nBodylen;				//消息体长度
	char m_nCmd;				//消息id

	enum ECdnCtlMgr
	{
		HEAD_LEN = 13,
		BODYLEN_OFFSET = 8,
		CMDID_OFFSET = 12
	};
	int Decode(CBufMgr& BufMgr)
	{
		if (BufMgr.GetInt(m_nProtocolVersion) != 0) {return -1;}
		m_nProtocolVersion = ntohl(m_nProtocolVersion);
		if (BufMgr.GetInt(m_nSeqence) != 0) {return -1;}
		m_nSeqence = ntohl(m_nSeqence);
		if (BufMgr.GetInt(m_nBodylen) != 0) {return -1;}
		m_nBodylen = ntohl(m_nBodylen);
		if (BufMgr.GetChar(m_nCmd) != 0) {return -1;}
		return 0;
	}
	int Encode(CBufMgr& BufMgr)
	{
		int nRet = 0;
		nRet += BufMgr.PutInt(htonl(m_nProtocolVersion));
		nRet += BufMgr.PutInt(htonl(m_nSeqence));
		nRet += BufMgr.PutInt(htonl(m_nBodylen));
		nRet += BufMgr.PutChar(m_nCmd);
		return nRet;
	}
	int GetSize()
	{
		return HEAD_LEN;
	}
};


template <class T1, class T2 = CDefaultRes>
class CSeedBaseCmd
	: public CCommand
{
public:
	static int GetCommandId(char* pBuf, int nLen) 
	{
		if(nLen < TSeedBaseCmd::HEAD_LEN) 
		{
			WriteRunInfo::WriteLog("CSeedBaseCmd::GetCommandId : cmd buf len is less than %d", TSeedBaseCmd::HEAD_LEN);
			return INVALID_CMD_ID;
		}

		unsigned char nCmdId;
		memcpy(&nCmdId, pBuf + TSeedBaseCmd::CMDID_OFFSET, sizeof(nCmdId));
		return	nCmdId;
	}
public:
	CSeedBaseCmd(char* pBuf, int nLen) : m_inBuf(NULL, 0, 1, 0), m_outBuf(NULL, 0, 1, 0)
	{
		m_outHead.m_nProtocolVersion = VOD_PROTOCOL_VER;
		m_inBuf.AttachBuf(pBuf, nLen);
		m_bIfDecode = false;
		m_nProcessRet = -1;
	}
	virtual ~CSeedBaseCmd()
	{

	}
public:
	TSeedBaseCmd& GetInCmdHead() {return m_inHead;} 
	T1& GetInCmdBody() {return m_inBody;}
	virtual void DisAttachBuf() 
	{
		m_outBuf.DistachBuf();
	}
	virtual char* GetResBuf(int& nBufLen)
	{
		nBufLen = m_outBuf.GetRealBufLen();
		return m_outBuf.GetBuf();
	}
	virtual short GetMsgType()
	{
		return m_inHead.m_nCmd;
	}
	int GetIp()
	{
		return m_nIp;
	}
	int DecodeDirect()
	{
		return Decode();
	}
	int EncodeDirect()
	{
		GenResHead();
		return Encode();
	}
	int ProcessOnly()
	{
		return ProcessCmd();
	}
protected:
	virtual int Process(int& nResult)
	{
		int nRet = Decode();
		if (nRet == 0)
		{
			m_nProcessRet =  ProcessCmd();
			nResult = m_nProcessRet;
			if (m_nProcessRet == 0 && m_nIfRes == 1)
			{   
				GenResHead();
				nRet = Encode();
			}
		}
		nResult = (int)((unsigned char)m_inHead.m_nCmd);
		return nRet;
	}
	virtual int Decode()
	{
		int nRet = 0;
		if (!m_bIfDecode)
		{
			m_bIfDecode = true;
			nRet = m_inHead.Decode(m_inBuf);
			if (nRet != 0)
			{
				WriteRunInfo::WriteError("CSeedBaseCmd::Decode : Decode Cmd Head Fail");
				return nRet;
			}
			if ((m_inHead.m_nBodylen + TSeedBaseCmd::CMDID_OFFSET) != m_inBuf.GetBufLen())
			{
				WriteRunInfo::WriteError("CSeedBaseCmd::Decode : body len(%d) is invalid", m_inHead.m_nBodylen);
				return -1;
			}
			nRet = m_inBody.Decode(m_inBuf);
		}
		return nRet;
	}
	virtual int GenResHead()
	{
		m_outHead.m_nProtocolVersion = m_inHead.m_nProtocolVersion;
		m_outHead.m_nSeqence = m_inHead.m_nSeqence;
		m_outHead.m_nBodylen = sizeof(m_outHead.m_nCmd) + m_outBody.GetSize();
		return 0;
	}
	virtual int Encode()
	{
		int nPacketLen = m_outHead.GetSize() + m_outHead.m_nBodylen - sizeof(m_outHead.m_nCmd);
		char* pOutBuf = new char[nPacketLen];
		m_outBuf.AttachBuf(pOutBuf, nPacketLen);
		int nRet = m_outHead.Encode(m_outBuf);
		nRet += m_outBody.Encode(m_outBuf);
		return nRet;
	}
protected:
	virtual int ProcessCmd() = 0;
protected:
	bool m_bIfDecode;
	int m_nProcessRet;
	CBufMgr m_inBuf;
	CBufMgr m_outBuf;
	TSeedBaseCmd m_inHead;
	TSeedBaseCmd m_outHead;
	T1 m_inBody;
	T2 m_outBody;
};




#endif



