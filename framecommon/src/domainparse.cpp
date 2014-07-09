/**
 *    @file       domainparse.cpp
 *
 *    @brief      
 *    @details
 *
 *    @version    1.0
 *    @date       01/14/2013 11:55:26 AM
 *
 *    @author     Theophilus (), wangtf418@gmail.com
 */
#include <arpa/inet.h>
#include <netdb.h>
#include <resolv.h>
#include <sys/select.h>
#include <stdlib.h>
#include "domainparse.h"
#include "toolkit.h"
#include "myfile.h"

namespace MYFRAMECOMMON{

const unsigned int MAX_IDENTIFIER_LEN = 63;
const unsigned short DEF_DNS_PORT = 53;

#ifndef _PATH_HOSTS
#define _PATH_HOSTS "/etc/hosts"
#endif

#ifndef _PATH_RESCONF
#define _PATH_RESCONF "/etc/resolv.conf"
#endif

int DecodeUrlFromDnsProtocal(char* pBuf, int nBufLen, unsigned short nOffset, std::string &strDomain)
{
	if (!pBuf || nOffset > (unsigned) nBufLen)
	{
		return -1;
	}

	char szBuf[MAX_IDENTIFIER_LEN];
	CBufMgr TmpBuf(pBuf + nOffset, nBufLen - nOffset, 0);
	unsigned char nTmp = 0;
	while (1)
	{
		if (TmpBuf.GetChar((char&)nTmp) != 0)
		{
			return -1;
		}
		if (nTmp > MAX_IDENTIFIER_LEN)
		{
			if ((nTmp & 0xC0) != 0xC0) return -1;

			unsigned short nOffset = nTmp & 0x3F;
			if (TmpBuf.GetChar((char&)nTmp) != 0)
			{
				return -1;
			}
			nOffset = (nOffset << 8) + nTmp;
			return DecodeUrlFromDnsProtocal(pBuf, nBufLen, nOffset, strDomain);
		}
		if (nTmp == 0)
		{
			break;
		}
		if (TmpBuf.GetCharBuf((char*)szBuf, nTmp) != 0)
		{
			return -1;
		}
		if (strDomain != "")
		{
			strDomain += ".";
		}
		strDomain += std::string(szBuf, nTmp);
	}
	return 0;
}

int TDnsProtocal::Encode(CBufMgr &Buffer)
{
	int nRet = 0;

	nRet += Buffer.PutShort(m_nId);
	nRet += Buffer.PutShort(m_nFlag);
	nRet += Buffer.PutShort(m_nQuestionNum);
	nRet += Buffer.PutShort(m_nRRNum);
	nRet += Buffer.PutShort(m_nAuthorRRNum);
	nRet += Buffer.PutShort(m_nAdditionRRNum);

	return nRet;
}

int TDnsProtocal::Decode(CBufMgr &Buffer)
{
	if (Buffer.GetShort((short&)m_nId) != 0) return -1;
	if (Buffer.GetShort((short&)m_nFlag) != 0) return -1;
	if (Buffer.GetShort((short&)m_nQuestionNum) != 0) return -1;
	if (Buffer.GetShort((short&)m_nRRNum) != 0) return -1;
	if (Buffer.GetShort((short&)m_nAuthorRRNum) != 0) return -1;
	if (Buffer.GetShort((short&)m_nAdditionRRNum) != 0) return -1;
	return 0;
}

int TDnsQuery::Encode(CBufMgr &Buffer)
{
	std::string strTmp;

	int nRet = 0;

	const char *pDomain	= m_strDomain.c_str();
	const char *pLast = pDomain;
	const char *pCur = pLast;
	while (*pCur != '\0')
	{
		if (*pCur != '.')
		{
			pCur ++;
			continue;
		}
		if (pCur == pLast)
		{
			return -1;
		}
		char nSepLen = pCur - pLast;
		if (nSepLen < 0 || (unsigned char)nSepLen > MAX_IDENTIFIER_LEN)
		{
			return -1;
		}
		nRet += Buffer.PutChar(nSepLen);
		nRet += Buffer.PutCharBuf(pLast, nSepLen);

		pCur ++;
		pLast = pCur;
	}

	if (pCur == pLast)
	{
		char nSepLen = 0;
		nRet += Buffer.PutChar(nSepLen);
	}
	else
	{
		char nSepLen = pCur - pLast;
		if (nSepLen < 0 || (unsigned char)nSepLen > MAX_IDENTIFIER_LEN)
		{
			return -1;
		}
		nRet += Buffer.PutChar(nSepLen);
		nRet += Buffer.PutCharBuf(pLast, nSepLen);

		nSepLen = 0;
		nRet += Buffer.PutChar(nSepLen);
	}

	nRet += Buffer.PutShort(m_nQueryType);
	nRet += Buffer.PutShort(m_nQueryAddrType);

	return nRet;
}

int TDnsQuery::Decode(CBufMgr &Buffer)
{
	unsigned char nTmp;
	char szBuf[MAX_IDENTIFIER_LEN];
	m_strDomain = "";
	while (1) 
	{
		if (Buffer.GetChar((char&)nTmp) != 0)
		{
			return -1;
		}
		if (nTmp > MAX_IDENTIFIER_LEN)
		{
			if ((nTmp & 0xC0) != 0xC0) return -1;

			unsigned short nOffset = nTmp & 0x3F;
			if (Buffer.GetChar((char&)nTmp) != 0)
			{
				return -1;
			}
			nOffset = (nOffset << 8) + nTmp;
			
			if (DecodeUrlFromDnsProtocal(Buffer.GetBuf(), Buffer.GetBufLen(), nOffset, m_strDomain) != 0)
			{
				return -1;
			}
			break;
		}
		if (nTmp == 0)
		{
			break;
		}
		if (Buffer.GetCharBuf((char*)szBuf, nTmp) != 0)
		{
			return -1;
		}
		if (m_strDomain != "")
		{
			m_strDomain += ".";
		}
		m_strDomain += std::string(szBuf, nTmp);
	}
	if (Buffer.GetShort((short&)m_nQueryType) != 0) return -1;
	if (Buffer.GetShort((short&)m_nQueryAddrType) != 0) return -1;
	return 0;
}

int TDnsQueryRes::Encode(CBufMgr &Buffer)
{
	return 0;
}

int TDnsQueryRes::Decode(CBufMgr &Buffer)
{
	unsigned char nTmp = 0;	
	char szBuf[MAX_IDENTIFIER_LEN];
	m_strDomain = "";
	while (1)
	{
		if (Buffer.GetChar((char&)nTmp) != 0)
		{
			return -1;
		}
		if (nTmp > MAX_IDENTIFIER_LEN)
		{
			if ((nTmp & 0xC0) != 0xC0) return -1;
			unsigned short nOffset = nTmp & 0x3F;
			if (Buffer.GetChar((char&)nTmp) != 0)
			{
				return -1;
			}
			nOffset = (nOffset << 8) + nTmp;


			if (DecodeUrlFromDnsProtocal(Buffer.GetBuf(), Buffer.GetBufLen(), nOffset, m_strDomain) != 0)
			{
				return -1;
			}
			break;
		}
		if (nTmp == 0)
		{
			break;
		}
		if (Buffer.GetCharBuf((char*)szBuf, nTmp) != 0)
		{
			return -1;
		}
		if (m_strDomain != "")
		{
			m_strDomain += ".";
		}
		m_strDomain += std::string(szBuf, nTmp);
	}

	if (Buffer.GetShort((short&)m_nQueryType) != 0) return -1;
	if (Buffer.GetShort((short&)m_nQueryAddrType) != 0) return -1;
	if (Buffer.GetInt((int&)m_nValidTime) != 0) return -1;
	if (Buffer.GetShort((short&)m_nDataLen) != 0) return -1;
	char *pBuf = new char[m_nDataLen];
	if (Buffer.GetCharBuf(pBuf, m_nDataLen) != 0)
	{
		delete [] pBuf;
		return -1;
	}
	m_strData = std::string(pBuf, m_nDataLen);
	delete [] pBuf;
	if (m_nQueryType == DMQ_NS || m_nQueryType == DMQ_CNAME || m_nQueryType == DMQ_PTR)
	{
		std::string strTmpDomain;
		if (DecodeUrlFromDnsProtocal(Buffer.GetBuf(), Buffer.GetBufLen(), Buffer.GetRealBufLen() - m_strData.length(), strTmpDomain) != 0)
		{
			return -1;
		}
		m_strData = strTmpDomain;
		//printf("type=%u,domain=%s\n", m_nQueryType, m_strData.c_str());
	}
	return 0;
}


CDomainParse::CDomainParse(bool bIfPrintRes /* = false */)
	: m_nTimeout(DEF_DOMAIN_PARSE_TIMEOUT), m_bIfPrintRes(bIfPrintRes)
{

}

CDomainParse::~CDomainParse()
{

}

int CDomainParse::GetHostByName(const std::string &strDomain, std::vector<unsigned int> &vecOutIp, std::string strDnsHost /* = "" */, unsigned int nTimeout /* = DEF_DOMAIN_PARSE_TIMEOUT */)
{
	vecOutIp.clear();

	if (IsIpString(strDomain))
	{
		unsigned int nIpOut;
		if (inet_pton(AF_INET, strDomain.c_str(), &nIpOut) > 0)
		{
			vecOutIp.push_back(ntohl(nIpOut));
			return 0;
		}
		else
		{
			return -1;
		}
	}

	unsigned int nIp = 0;
	if (ParseDomainByHostsFile(strDomain, nIp) == 0)
	{
		vecOutIp.push_back(nIp);
		return 0;
	}

	std::string strDnsIp = strDnsHost;
	if (strDnsHost != "" && !IsIpString(strDnsHost))
	{
		std::vector<unsigned int> vecDnsHost;
		if (ParseDomain(strDnsHost, vecDnsHost) != 0)
		{
			return -1;
		}
		if (vecDnsHost.empty())
		{
			return -1;
		}
		nIp = htonl(vecDnsHost[0]);
		char szIp[16];
		inet_ntop(AF_INET, &nIp, szIp, sizeof(szIp));
		strDnsIp = szIp;
	}

	return ParseDomain(strDomain, vecOutIp, strDnsIp, nTimeout);
}

int CDomainParse::ParseDomain(const std::string &strDomain, std::vector<unsigned int> &vecOutIp, std::string strDnsIp /* = "" */, unsigned int nTimeout /* = DEF_DOMAIN_PARSE_TIMEOUT */)
{
	if (strDomain == "")
	{
		return -1;
	}
	std::vector<unsigned int> vecDnsIp;
	std::string strAddr = strDnsIp;
	if (strAddr == "")
	{
		GetLocalDnsIp(vecDnsIp);
	}
	else
	{
		unsigned int nIpOut;
		if (inet_pton(AF_INET, strAddr.c_str(), &nIpOut) > 0)
		{
			vecDnsIp.push_back(ntohl(nIpOut));
		}
	}
	if (vecDnsIp.size() <= 0)
	{
		return -1;
	}

	TDnsProtocal DnsProtocal;

	TDnsProtocalFlag DnsProtocalFlag;
	DnsProtocalFlag.m_nType = 0;
	DnsProtocalFlag.m_nOperation = 0;
	DnsProtocalFlag.m_nIfAuthor = 0;
	DnsProtocalFlag.m_nITruncated = 1;
	DnsProtocalFlag.m_nIfRecursionDesired = 1;
	DnsProtocalFlag.m_nRecursionAvaliable = 0;
	DnsProtocalFlag.m_nZero = 0;
	DnsProtocalFlag.m_nRetCode = 0;

	DnsProtocal.m_nId = 0;
	DnsProtocal.m_nFlag = DnsProtocalFlag.CalcFlag();
	DnsProtocal.m_nQuestionNum = 1;
	DnsProtocal.m_nRRNum = 0;
	DnsProtocal.m_nAuthorRRNum = 0;
	DnsProtocal.m_nAdditionRRNum = 0;

	TDnsQuery DnsQuery;
	DnsQuery.m_strDomain = strDomain;
	DnsQuery.m_nQueryType = DMQ_A;
	DnsQuery.m_nQueryAddrType = 1;

	int nSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (nSocket < 0)
	{
		return -1;
	}

	int nDnsNum = (int)vecDnsIp.size();
	for (int nA = 0; nA < nDnsNum; nA ++)
	{
		DnsProtocal.m_nId = nA;
		CDnsCmd DnsCmd(DnsProtocal, DnsQuery);
		SendRequest(nSocket, vecDnsIp[nA], DEF_DNS_PORT, DnsCmd);
	}

	int nBeginTime = time(NULL);
	std::map<int, CDnsCmdRes> mapCmdRes;
	while (1)
	{
		char *pRecvBuf = new char[1024];
		CBufMgr TmpBufRes(pRecvBuf, 1024);
		CDnsCmdRes DnsCmdRes;
		int nRet = RecvResponse(nSocket, pRecvBuf, 1024, DnsCmdRes, 2);
		if (nRet == 0)
		{
			unsigned short nProtocalId = DnsCmdRes.GetDnsProtocal().m_nId;
			if (nProtocalId >= (unsigned)nDnsNum)
			{
				continue;
			}
			if (mapCmdRes.find(nProtocalId) == mapCmdRes.end()) 
			{
				mapCmdRes.insert(make_pair(nProtocalId, DnsCmdRes));
				if (m_bIfPrintRes)
				{
					DnsCmdRes.Print();
				}
				
				if (nProtocalId == 0)
				{
					break;
				}
			}
		}
		else if (nRet == -1)
		{
			break;
		}
		else if (nRet == -2)
		{
			int nCurTime = time(NULL);
			if (nCurTime - nBeginTime > (int)nTimeout)
			{
				break;
			}
			for (int nA = 0; nA < nDnsNum; nA ++)
			{
				if (mapCmdRes.find(nA) != mapCmdRes.end()) continue;
				DnsProtocal.m_nId = nA;
				CDnsCmd DnsCmd(DnsProtocal, DnsQuery);
				SendRequest(nSocket, vecDnsIp[nA], DEF_DNS_PORT, DnsCmd);
				if (m_bIfPrintRes)
				{
					printf("re send %d\n", nA);
				}
			}
		}
		else
		{
			continue;
		}
	}
	if (mapCmdRes.size() > 0)
	{
		mapCmdRes.begin()->second.GetAddress(vecOutIp);
		if (m_bIfPrintRes)
		{
			printf("result>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
			mapCmdRes.begin()->second.Print();
		}
	}
	

	return 0;
}

int CDomainParse::ParseDomainByHostsFile(const std::string &strDomain, unsigned int &nIp)
{
	CMyFile HostFile(_PATH_HOSTS);
	struct stat statFile;
	int nRet = stat(_PATH_HOSTS, &statFile);
	if (nRet != 0)
	{
		return -1;
	}
	int nFileSize = (int)statFile.st_size;
	char *pBuf = new char[nFileSize];
	CBufMgr TmpBuf(pBuf, nFileSize);
	try 
	{
		HostFile.FOpen("r");
		HostFile.FRead(pBuf, nFileSize);
		HostFile.FClose();
	}
	catch(CMyException &e)
	{
		return -1;
	}
	std::string strOutIp;
	int nPos = 0;
	int nLastPos = 0;
	while (nPos < nFileSize)
	{
		nLastPos = nPos;
		nPos = CToolKit::GetStringByLabel(std::string(pBuf, nFileSize), "", "\n", strOutIp, nLastPos, 1);
		if (nPos < 0)
		{
			strOutIp = std::string(pBuf + nLastPos, nFileSize - nLastPos);
			nPos = nFileSize;
		}
		CToolKit::StringTrim(strOutIp);
		if (strOutIp.empty() || strOutIp[0] == '#' || strOutIp[0] == ';')
		{
			continue;
		}

		if(CToolKit::GetStringByLabel(strOutIp, "", strDomain, strOutIp, 0, 1) < 0)
		{
			continue;
		}
		CToolKit::StringTrim(strOutIp);
		if (strOutIp == "")
		{
			continue;
		}

		if (inet_pton(AF_INET, strOutIp.c_str(), &nIp) <= 0)
		{
			continue;
		}
		else
		{
			nIp = ntohl(nIp);
			return 0;
		}
	}
	return -1;
}

int CDomainParse::GetLocalDnsIp(std::vector<unsigned int> &vecOutIp)
{
	vecOutIp.clear();
	CMyFile DnsFile(_PATH_RESCONF);
	struct stat statFile;
	int nRet = stat(_PATH_RESCONF, &statFile);
	if (nRet != 0)
	{
		return -1;
	}

	int nFileSize = (int)statFile.st_size;
	char *pBuf = new char[nFileSize];
	CBufMgr TmpBuf(pBuf, nFileSize);

	try 
	{
		DnsFile.FOpen("r");
		DnsFile.FRead(pBuf, nFileSize);
		DnsFile.FClose();
	}
	catch(CMyException &e)
	{
		return -1;
	}

	std::string strOutIp;
	unsigned int nIp = 0;
	int nPos = 0;
	int nLastPos = 0;
	while (nPos < nFileSize)
	{
		nLastPos = nPos;
		nPos = CToolKit::GetStringByLabel(std::string(pBuf, nFileSize), "", "\n", strOutIp, nLastPos, 1);
		if (nPos < 0)
		{
			strOutIp = std::string(pBuf + nLastPos, nFileSize - nLastPos);
			nPos = nFileSize;
		}
		CToolKit::StringTrim(strOutIp);
		if (strOutIp.empty() || strOutIp[0] == '#' || strOutIp[0] == ';')
		{
			continue;
		}

		if(CToolKit::GetStringByLabel(strOutIp, "nameserver", "", strOutIp, 0, 1) < 0)
		{
			continue;
		}
		CToolKit::StringTrim(strOutIp);
		if (strOutIp == "")
		{
			continue;
		}

		if (inet_pton(AF_INET, strOutIp.c_str(), &nIp) <= 0)
		{
			continue;
		}
		else
		{
			nIp = ntohl(nIp);
			vecOutIp.push_back(nIp);
		}
	}
	return 0;
}

int CDomainParse::SendRequest(int nSocket, unsigned int nIp, unsigned short nPort, CDnsCmd &DnsCmd)
{
	if (nSocket < 0)
	{
		return -1;
	}

	char *pBuf = new char[1024];
	CBufMgr TmpBuf(pBuf, 1024);

	int nRet = DnsCmd.Encode(TmpBuf);
	if (nRet != 0)
	{
		return -1;
	}

	sockaddr_in saDnsSrv;
	saDnsSrv.sin_family = AF_INET;
	saDnsSrv.sin_addr.s_addr = htonl(nIp);
	saDnsSrv.sin_port = htons(nPort);
	
	unsigned int nSockLen = sizeof(saDnsSrv);
	nRet = sendto(nSocket, pBuf, TmpBuf.GetRealBufLen(), 0, (sockaddr*)&saDnsSrv, nSockLen);
	if (nRet < 0)
	{
		return -1;
	}
	return 0;
}

int CDomainParse::RecvResponse(int nSocket, char *pOutBuf, int nBufLen, CDnsCmdRes &DnsCmdRes, int nTimeout /* = 2 */)
{
	if (nSocket < 0)
	{
		return -1;
	}

	char *pBuf = new char[1024];
	CBufMgr TmpBuf(pBuf, 1024);

	char *pRecvBuf = new char[1024];
	CBufMgr TmpBufRes(pRecvBuf, 1024);

	sockaddr_in saFrom;
	unsigned int nFromSockLen = sizeof(saFrom);

	fd_set setRead;
	struct timeval tvSelectWait;
	tvSelectWait.tv_sec = nTimeout;
	tvSelectWait.tv_usec = 0;
	int nRet = 0;
	while (1)
	{
		FD_ZERO(&setRead);
		FD_SET(nSocket, &setRead);
		TmpBufRes.AttachBuf(pRecvBuf, 1024);
		if((nRet = select(nSocket + 1, &setRead,  NULL, NULL, &tvSelectWait)) < 0)
		{
			if(errno == EINTR) continue;
			return -1; //socket error
		}

		if (nRet == 0)
		{
			return -2; //timeout
		}

		int nRecvLen = recvfrom(nSocket, pRecvBuf, 1024, 0, (sockaddr*)&saFrom, &nFromSockLen);
		if (nRecvLen <= 0)
		{
			return -1; //socket error
		}
		TmpBufRes.AttachBuf(pRecvBuf, nRecvLen);

		int nRet = DnsCmdRes.Decode(TmpBufRes);
		if (nRet != 0)
		{
			return -3; //other error
		}
		else
		{
			break;
		}
	}
	return 0;
}

bool CDomainParse::IsIpString(const std::string &strIpStr)
{
	if (strIpStr.length() < 7) //1.0.0.0 
	{
		return false;
	}
	int nStrLen = (int)strIpStr.length();
	for (int nA = 0; nA < nStrLen; nA ++)
	{
		if (strIpStr[nA] >= '0' && strIpStr[nA] <= '9')
		{
			continue;
		}
		if (strIpStr[nA] != '.')
		{
			return false;
		}
	}
	std::vector<std::string> vecOut;
	CToolKit::SplitString(strIpStr, ".", vecOut);
	if (vecOut.size() != 4)
	{
		return false;
	}
	for (int nA = 0; nA < 4; nA ++)
	{
		long int nNum = strtol(vecOut[nA].c_str(), NULL, 10);
		if (nNum < 0 || nNum > 255)
		{
			return false;
		}
	}
	return true;
}

} //end namespace


