/**
 *    @file       domainparse.h
 *
 *    @brief      
 *    @details
 *
 *    @version    1.0
 *    @date       01/14/2013 11:50:59 AM
 *
 *    @author     Theophilus (), wangtf418@gmail.com
 */
#ifndef _DOMAINPARSE_H_854C2572_D676_4E98_A2F2_4FC47CE9EEDF_
#define _DOMAINPARSE_H_854C2572_D676_4E98_A2F2_4FC47CE9EEDF_
#include <string>
#include <vector>
#include "buffermgr.h"

namespace MYFRAMECOMMON{

const unsigned int DEF_DOMAIN_PARSE_TIMEOUT = 10;

enum EDOMAINPARSE_QUERYTYPE
{
	DMQ_A = 1,
	DMQ_NS = 2,
	DMQ_CNAME = 5,
	DMQ_PTR = 12,
	DMQ_HINFO = 13,
	DMQ_MX = 15,
	DMQ_AXFR = 252,
	DMQ_ANY = 255
};

struct TDnsProtocal
{
	unsigned short m_nId;				//protocal id
	unsigned short m_nFlag;				//protocal flag
	unsigned short m_nQuestionNum;		//append question num
	unsigned short m_nRRNum;			//Resource Record num
	unsigned short m_nAuthorRRNum;		//Authoritative Resource Record num
	unsigned short m_nAdditionRRNum;	//Addition Resource Record num
	int Encode(CBufMgr &Buffer);
	int Decode(CBufMgr &Buffer);
	void Print()
	{
		printf("m_nId=%u\n", m_nId);
		printf("m_nFlag=%u\n", m_nFlag);
		printf("m_nQuestionNum=%u\n", m_nQuestionNum);
		printf("m_nRRNum=%u\n", m_nRRNum);
		printf("m_nAuthorRRNum=%u\n", m_nAuthorRRNum);
		printf("m_nAdditionRRNum=%u\n", m_nAdditionRRNum);

	}
};

struct TDnsProtocalFlag
{
		unsigned short m_nType;						//:1;	//0,query;1,answer
		unsigned short m_nOperation;				//:4;	//0,standard query; 1,reverse query; 2,srv status query
		unsigned short m_nIfAuthor;					//:1;
		unsigned short m_nITruncated;				//:1;
		unsigned short m_nIfRecursionDesired;		//:1;

		unsigned short m_nRecursionAvaliable;		//:1;
		unsigned short m_nZero;						//:3;	
		unsigned short m_nRetCode;					//:4;	//0,ok;3,name error

		unsigned short CalcFlag()
		{
			unsigned short nFlag = 0;

			m_nType = (m_nType << 15) & 0x8000;
			m_nOperation = (m_nOperation << 11) & 0x7800;
			m_nIfAuthor = (m_nIfAuthor << 10) & 0x0400;
			m_nITruncated = (m_nITruncated << 9) & 0x0200;
			m_nIfRecursionDesired = (m_nIfRecursionDesired << 8) & 0x0100;

			m_nRecursionAvaliable = (m_nRecursionAvaliable << 7) & 0x0080;
			m_nZero = (m_nZero << 4) & 0x0070;
			m_nRetCode = m_nRetCode & 0x000F;

			nFlag = m_nType | m_nOperation | m_nIfAuthor | m_nITruncated | m_nIfRecursionDesired | m_nRecursionAvaliable | m_nZero | m_nRetCode;
			return nFlag;
		}

		void ParseFlag(unsigned short nFlag)
		{
			m_nType = (nFlag & 0x8000) >> 15;
			m_nOperation = (nFlag & 0x7800) >> 11;
			m_nIfAuthor = (nFlag & 0x0400) >> 10;
			m_nITruncated = (nFlag & 0x0200) >> 9;
			m_nIfRecursionDesired = (nFlag & 0x0100) >> 8;

			m_nRecursionAvaliable = (nFlag & 0x0080) >> 7;
			m_nZero= (nFlag & 0x0070) >> 4;
			m_nRetCode = (nFlag & 0x000F);
		}

		void Print()
		{
			printf("flag --------------\n");
			printf("\tm_nType=%u\n", m_nType);
			printf("\tm_nOperation=%u\n", m_nOperation);
			printf("\tm_nIfAuthor=%u\n", m_nIfAuthor);
			printf("\tm_nITruncated=%u\n", m_nITruncated);
			printf("\tm_nIfRecursionDesired=%u\n", m_nIfRecursionDesired);
			printf("\tm_nRecursionAvaliable=%u\n", m_nRecursionAvaliable);
			printf("\tm_nZero=%u\n", m_nZero);
			printf("\tm_nRetCode=%u\n", m_nRetCode);
		}
};

struct TDnsQuery
{
	std::string m_strDomain;
	unsigned short m_nQueryType;		//EDOMAINPARSE_QUERYTYPE
	unsigned short m_nQueryAddrType;	//1,internet address
	int Encode(CBufMgr &Buffer);
	int Decode(CBufMgr &Buffer);
	void Print()
	{
		printf("Query ---------------\n");
		printf("\tm_strDomain=%s\n", m_strDomain.c_str());
		printf("\tm_nQueryType=%u\n", m_nQueryType);
		printf("\tm_nQueryAddrType=%u\n", m_nQueryAddrType);
	}
};

struct TDnsQueryRes
{
	std::string m_strDomain;
	unsigned short m_nQueryType;		//EDOMAINPARSE_QUERYTYPE
	unsigned short m_nQueryAddrType;	//1,internet address
	unsigned int m_nValidTime;
	unsigned short m_nDataLen;
	std::string m_strData;

	int Encode(CBufMgr &Buffer);
	int Decode(CBufMgr &Buffer);
	void Print()
	{
		printf("QueryRes ---------------\n");
		printf("\tm_strDomain=%s\n", m_strDomain.c_str());
		printf("\tm_nQueryType=%u\n", m_nQueryType);
		printf("\tm_nQueryAddrType=%u\n", m_nQueryAddrType);
		printf("\tm_nValidTime=%u\n", m_nValidTime);
		printf("\tm_nDataLen=%u\n", m_nDataLen);

		if (m_nQueryType == DMQ_NS || m_nQueryType == DMQ_CNAME || m_nQueryType == DMQ_PTR)
		{
			printf("\tm_strData=%s\n", m_strData.c_str());
		}
		else if (m_nQueryType == DMQ_A && m_nDataLen == 4)
		{
			unsigned int nIp;
			memcpy(&nIp, m_strData.c_str(), 4);
			char szBuf[16];
			inet_ntop(AF_INET, &nIp, szBuf, sizeof(szBuf));
			printf("\tm_strData=%s\n", szBuf);
		}
	}
};

class CDnsCmd
{
public:
	CDnsCmd()
	{

	}
	CDnsCmd(const TDnsProtocal &DnsProtocal, const TDnsQuery &DnsQuery)
	{
		m_DnsProtocal = DnsProtocal;
		m_DnsQuery = DnsQuery;
	}
	~CDnsCmd() {}
public:
	void SetQuery(const TDnsProtocal &DnsProtocal, const TDnsQuery &DnsQuery)
	{
		m_DnsProtocal = DnsProtocal;
		m_DnsQuery = DnsQuery;
	}
public:
	int Encode(CBufMgr &Buffer)
	{
		int nRet = m_DnsProtocal.Encode(Buffer);
		m_DnsProtocalFlag.ParseFlag(m_DnsProtocal.m_nFlag);
		nRet += m_DnsQuery.Encode(Buffer);
		return nRet;
	}
	int Decode(CBufMgr &Buffer)
	{
		return 0;
	}
public:
	TDnsProtocal& GetDnsProtocal()
	{
		return m_DnsProtocal;
	}
	TDnsProtocalFlag& GetDnsProtocalFlag()
	{
		return m_DnsProtocalFlag;
	}
	TDnsQuery& GetDnsQuery()
	{
		return m_DnsQuery;
	}
private:
	TDnsProtocal m_DnsProtocal;
	TDnsProtocalFlag m_DnsProtocalFlag;
	TDnsQuery m_DnsQuery;
};

class CDnsCmdRes
{
public:
	CDnsCmdRes() {}
	~CDnsCmdRes() {}
public:
	int Encode(CBufMgr &Buffer)
	{
		return 0;
	}
	int Decode(CBufMgr &Buffer)
	{
		if (m_DnsProtocal.Decode(Buffer) != 0) return -1;
		m_DnsProtocalFlag.ParseFlag(m_DnsProtocal.m_nFlag);

		m_vecDnsQuery.clear();
		for (int nA = 0; nA < m_DnsProtocal.m_nQuestionNum; nA ++)
		{
			TDnsQuery TmpDnsQuery;
			if (TmpDnsQuery.Decode(Buffer) != 0) return -1;
			m_vecDnsQuery.push_back(TmpDnsQuery);
		}

		m_vecRcRecord.clear();
		for (int nA = 0; nA < m_DnsProtocal.m_nRRNum; nA ++)
		{
			TDnsQueryRes TmpDnsQueryRes;
			if (TmpDnsQueryRes.Decode(Buffer) != 0) return -1;
			m_vecRcRecord.push_back(TmpDnsQueryRes);
		}

		m_vecAuthRcRecord.clear();
		for (int nA = 0; nA < m_DnsProtocal.m_nAuthorRRNum; nA ++)
		{
			TDnsQueryRes TmpDnsQueryRes;
			if (TmpDnsQueryRes.Decode(Buffer) != 0) return -1;
			m_vecAuthRcRecord.push_back(TmpDnsQueryRes);
		}

		m_vecExRcRecord.clear();
		for (int nA = 0; nA < m_DnsProtocal.m_nAdditionRRNum; nA ++)
		{
			TDnsQueryRes TmpDnsQueryRes;
			if (TmpDnsQueryRes.Decode(Buffer) != 0) return -1;
			m_vecExRcRecord.push_back(TmpDnsQueryRes);
		}
		return 0;
	}
	void GetAddress(std::vector<unsigned int>& vecOutIp)
	{
		unsigned int nIp = 0;
		int nRecordNum = m_vecRcRecord.size();
		for (int nA = 0; nA < nRecordNum; nA ++)
		{
			if (m_vecRcRecord[nA].m_nQueryType == DMQ_A)
			{
				memcpy(&nIp, m_vecRcRecord[nA].m_strData.c_str(), 4);
				vecOutIp.push_back(htonl(nIp));
			}
		}
	}
	void Print()
	{
		m_DnsProtocal.Print();
		m_DnsProtocalFlag.Print();
		int nNum = (int)m_vecDnsQuery.size();
		printf("query:%d=================\n", nNum);
		for (int nA = 0; nA < nNum; nA ++)
		{
			m_vecDnsQuery[nA].Print();
		}

		nNum = (int)m_vecRcRecord.size();
		printf("rc record:%d=================\n",nNum);
		for (int nA = 0; nA < nNum; nA ++)
		{
			m_vecRcRecord[nA].Print();
		}

		nNum = (int)m_vecAuthRcRecord.size();
		printf("auth record:%d=================\n", nNum);
		for (int nA = 0; nA < nNum; nA ++)
		{
			m_vecAuthRcRecord[nA].Print();
		}

		nNum = (int)m_vecExRcRecord.size();
		printf("ex record:%d=================\n", nNum);
		for (int nA = 0; nA < nNum; nA ++)
		{
			m_vecExRcRecord[nA].Print();
		}
	}
public:
	TDnsProtocal& GetDnsProtocal()
	{
		return m_DnsProtocal;
	}
	TDnsProtocalFlag& GetDnsProtocalFlag()
	{
		return m_DnsProtocalFlag;
	}
	vector<TDnsQuery>& GetDnsQuery()
	{
		return m_vecDnsQuery;
	}
	vector<TDnsQueryRes>& GetRcRecord()
	{
		return m_vecRcRecord;
	}
	vector<TDnsQueryRes>& GetAuthRcRecord()
	{
		return m_vecAuthRcRecord;
	}
	vector<TDnsQueryRes>& GetExRcRecord()
	{
		return m_vecExRcRecord;
	}

private:
	TDnsProtocal m_DnsProtocal;
	TDnsProtocalFlag m_DnsProtocalFlag;
	vector<TDnsQuery> m_vecDnsQuery;
	vector<TDnsQueryRes> m_vecRcRecord;
	vector<TDnsQueryRes> m_vecAuthRcRecord;
	vector<TDnsQueryRes> m_vecExRcRecord;
};


class CDomainParse
{
public:
	CDomainParse(bool bIfPrintRes = false);
	~CDomainParse();

public:
	int GetHostByName(const std::string &strDomain, std::vector<unsigned int> &vecOutIp, std::string strDnsHost = "", unsigned int nTimeout = DEF_DOMAIN_PARSE_TIMEOUT);
	int ParseDomainByHostsFile(const std::string &strDomain, unsigned int &nIp);
	int ParseDomain(const std::string &strDomain, std::vector<unsigned int> &vecOutIp, std::string strDnsIp = "", unsigned int nTimeout = DEF_DOMAIN_PARSE_TIMEOUT);
	int GetLocalDnsIp(std::vector<unsigned int> &vecOutIp);

private:
	int SendRequest(int nSocket, unsigned int nIp, unsigned short nPort, CDnsCmd &DnsCmd);
	int RecvResponse(int nSocket, char *pOutBuf, int nBufLen, CDnsCmdRes &DnsCmdRes, int nTimeout = 2);
	bool IsIpString(const std::string &strIpStr);

private:
	std::string m_strDomain;
	unsigned int m_nTimeout;
	bool m_bIfPrintRes;
};

} //end namespace
#endif

