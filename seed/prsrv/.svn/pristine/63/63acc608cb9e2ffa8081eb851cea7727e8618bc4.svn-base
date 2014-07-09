#ifndef __PBASECOMMOND_H_XXXXXXXXXXXCCCCCCCCCCCCCCJJJJJJJJJJJJ
#define __PBASECOMMOND_H_XXXXXXXXXXXCCCCCCCCCCCCCCJJJJJJJJJJJJ

#include "framecommon/framecommon.h"
#include <string>
#include <vector>
#include "protocol.h"
#include "framecommon/buffermgr.h"
#include "psvrcomdef.h"
using namespace MYFRAMECOMMON;

const int NEW_VERSION102 = 102;
const unsigned char g_nDownPercentUnknow = -1;
const int g_nMaxFileBlockNum = 500;

namespace PHUB {

//*******************************************/
const int REQHEADLEN=13;
const int CMDIDOFF=12;

const unsigned int PEERLENGTH=16; //peeridlen
const unsigned int CIDLEN=20;    //cidlen

/****************PSeed*************************/
//define command id
const int ISRCONLINEREQ=ISRC_ONLINE;
const int ISRCONLINERESP=ISRC_ONLINE_RESP;
const int REPORTRCLISTREQ=REPORT_RCLIST;
const int REPORTRCLISTRESP=REPORT_RCLIST_RESP;
const int INSERTRCREQ=INSERT_RC;
const int INSERTRCRESP=INSERT_RC_RESP;
const int DELETERCREQ=DELETE_RC;
const int DELETERCRESP=DELETE_RC_RESP;
const int PEERQUERYREQ=PEER_QUERY;
const int PEERQUERYRESP=PEER_QUERY_RESP;
const int INVALIDPEERREQ = INVALID_PEER;
//-------------PTracker--------------------------
const int QUERY_TRACKER = 50;
const int QUERY_RES_TRACKER = 51;
const int DELETE_TRACKER = 52;
const int DELETE_RES_TRACKER = 53;
const int INVALID_PEER_TRACKER = 11;
//------------mvSeed-----------------------------
const int ISRC_ONLINE_MV = 101;
const int ISRC_ONLINE_RES_MV = 102;
const int REPORT_RCLIST_MV = 103;
const int REPORT_RCLIST_RES_MV = 104;
const int INSERT_RC_MV = 105;
const int INSERT_RC_RES_MV = 106;
const int QUERY_MPR_MV = 107;
const int QUERY_MPR_RES_MV = 108;
const int DELETE_RC_LIST = 109;
const int QUERY_MPR_NUM_MV = 111;
const int QUERY_MPR_NUM_RES_MV = 112;

const int INVALID_CMD_ID=-1; //zz

struct CBaseCommandHead
{
		int m_nProtocolVersion; //协议版本
		int m_nSeqence; //消息序列号
		int m_nBodylen; //消息体长度
		char m_nCmd;
};

struct CDefault
{
	int Decode (CBufMgr &bufMgr)
	{
		return 0;
	}
};

struct CDefaultRes
{
	int Encode(CTmpBufMgr &bufMgr)
	{
		return 0;
	}
    int GetSize()
    {
        return 0;
    }
};

class CCommand
{
    public:
        CCommand(){m_nIfRes=1;}
        virtual ~CCommand(){}
        virtual int Process(int &nResult)=0;
		void SetIp(int nIp)
		{
			m_nIp = nIp;
		}
		virtual char *GetResBuf(int &nLen)=0;
		virtual char GetMsgType()=0;
		virtual void DisAttachBuf()=0;
        int GetIfRes()
        {
            return m_nIfRes;
        }
    protected:
        virtual int Encode()=0;
        virtual int Decode()=0;
		int m_nIp;
        int m_nIfRes;
};

template <class T1, class T2=CDefaultRes> 
class CBaseCommand:public CCommand
{
	public:
		static int GetCommandId(char *pBuf, int nLen) //zz
		{
			if(nLen < REQHEADLEN) 
			{
				WriteRunInfo::WriteLog("The cmd buf len is less than %d, "
						"too short", REQHEADLEN);
				return INVALID_CMD_ID;
			}

			//unsigned short sCmdId;
			char sCmdId;
			memcpy(&sCmdId, pBuf + CMDIDOFF, sizeof(sCmdId));
			//return	ntohs(sCmdId);
			return sCmdId;
		}
		CBaseCommand(char *pBuf, int nLen):m_outBuf( NULL , 0 , 0)
		{
			m_outHead.m_nProtocolVersion = NEW_CLIENT_VOD_PROTOCOL_VER;
			m_inBuf.AttachBuf(pBuf, nLen);
            m_nIfDecode=0;
            m_nResult = -1;
            m_inBuf.SetIfNetTrans(0);
            m_outBuf.SetIfNetTrans(0);
		}
		virtual ~CBaseCommand(){}

		int Process2()
		{
			return ProcessCmd();
		}

		virtual int Process(int &nResult)
		{
			//解码
			nResult = 0;
		    int nRet=Decode();
            if (nRet == 0)
            {
                nResult =  ProcessCmd();
                m_nResult = nResult;
                if (nResult == 1)
                    m_nResult = 0;
            }
            
            if (nResult <= 1 && m_nIfRes==1) //>0一般是暂时不需要进行处理
            {   
                GenResHead(); //生成返回的头部
                nRet=Encode();
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

		char GetMsgType()
		{
			return m_inHead.m_nCmd;
		}
	protected:
        void GenResHead()
        {
            m_outHead.m_nProtocolVersion = m_inHead.m_nProtocolVersion; 
            m_outHead.m_nSeqence = m_inHead.m_nSeqence;
            m_outHead.m_nBodylen = 2;
            if (m_nResult == 0)
            {
                m_outHead.m_nBodylen += m_out.GetSize();
            }
        }
		int Encode()
		{
			int iPacketLength = sizeof(SERVERINNERCOMMANDPACKET) + m_out.GetSize() + 3*sizeof(int) + sizeof(char) + sizeof( char );
			char * pEncodeBuf = new char[iPacketLength];
			m_outBuf.AttachBuf( pEncodeBuf , iPacketLength );
			m_outBuf.SetOffset( sizeof(SERVERINNERCOMMANDPACKET) );

			int ret=0;
			ret+=m_outBuf.PutInt(htonl(m_outHead.m_nProtocolVersion));  //modified by dijie  09-11-16 应张旭需求，由于只有 包头 需要做字节序转换，所以作此修改
			ret+=m_outBuf.PutInt(htonl(m_outHead.m_nSeqence));  //modified by dijie  09-11-16 应张旭需求，由于只有 包头 需要做字节序转换，所以作此修改
			ret+=m_outBuf.PutInt(htonl(m_outHead.m_nBodylen));  //modified by dijie  09-11-16 应张旭需求，由于只有 包头 需要做字节序转换，所以作此修改
			ret+=m_outBuf.PutChar(m_outHead.m_nCmd);
			ret+=m_outBuf.PutChar(m_nResult);
			if (m_nResult == 0)
			{				
				ret += m_out.Encode(m_outBuf);
			}
			return ret;
		}
		int Decode()
		{
			int ret=0;
            if (m_nIfDecode == 0)
            {
				ret+=m_inBuf.GetInt(m_inHead.m_nProtocolVersion);   //modified by dijie  09-11-16 应张旭需求，由于只有 包头 需要做字节序转换，所以作此修改
				m_inHead.m_nProtocolVersion = ntohl(m_inHead.m_nProtocolVersion);
				ret+=m_inBuf.GetInt(m_inHead.m_nSeqence); //modified by dijie  09-11-16 应张旭需求，由于只有 包头 需要做字节序转换，所以作此修改
				m_inHead.m_nSeqence = ntohl(m_inHead.m_nSeqence);
				ret+=m_inBuf.GetInt(m_inHead.m_nBodylen); //modified by dijie  09-11-16 应张旭需求，由于只有 包头 需要做字节序转换，所以作此修改
				m_inHead.m_nBodylen = ntohl(m_inHead.m_nBodylen);
				ret+=m_inBuf.GetChar(m_inHead.m_nCmd);
				if (m_inHead.m_nBodylen + 12 != m_inBuf.GetBufLen())
				{
					WriteRunInfo::WriteLog("The body len is invalid, %d != %d",
							m_inHead.m_nBodylen + 12, m_inBuf.GetBufLen());
					return -1;
				}
				ret+=m_in.Decode(m_inBuf);	
	            m_nIfDecode = 1;
            }            
			return ret;
		}


		virtual int ProcessCmd() = 0;
		int m_nIfDecode;
		CBaseCommandHead m_inHead;	
		CBaseCommandHead m_outHead;	
		CBufMgr m_inBuf;
		CTmpBufMgr m_outBuf;
		char m_nResult;
		T1 m_in;
		T2 m_out;
};

}

#endif

