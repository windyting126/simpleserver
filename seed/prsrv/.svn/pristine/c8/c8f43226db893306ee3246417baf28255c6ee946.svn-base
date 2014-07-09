#include <utility>
#include "psvrcomdef.h"

using namespace std;

PPROTOMSGNAMEITEM g_MsgName[] =
{
	{1, "ISRC_ONLINE"},
	{2, "ISRC_ONLINE_RESP"},
	{3, "REPORT_RCLIST"},
	{4, "REPORT_RCLIST_RESP"},
	{5, "INSERT_RC"},
	{6, "INSERT_RC_RESP"},
	{7, "DELETE_RC"},
	{8, "DELETE_RC_RESP"},
	{9, "PEER_QUERY"},
	{10, "PEER_QUERY_RESP"},
	{11, "INVALID_PEER"},

	//Tracker Command
	{50, "QUERY_TRACKER"},
	{51, "QUERY_RES_TRACKER"},
	{52, "DELETE_TRACKER"},
	{53, "DELETE_RES_TRACKER"},

	//MV Command
	{101, "ISRC_ONLINE_MV"},
	{102, "ISRC_ONLINE_RES_MV"},
	{103, "REPORT_RCLIST_MV"},	
	{104, "REPORT_RCLIST_RES_MV"},
	{105, "INSERT_RC_MV"},
	{106, "INSERT_RC_RES_MV"},
	{107, "QUERY_MPR_MV"},
	{108, "QUERY_MPR_RES_MV"},
	{109, "DELETE_RC_LIST"},
	{111, "QUERY_MPR_PEER_NUM_MV"},
	{111, "QUERY_MPR_PEER_NUM_RES_MV"}
};

char *GetMsgNameByID(int msgID)
{
	for(unsigned int i=0; i<sizeof(g_MsgName)/sizeof(PPROTOMSGNAMEITEM); ++i)
	{
		if(msgID == g_MsgName[i].iMsgID)
			return g_MsgName[i].pMsgName;
	}

	return "Invalid_MsgID_For_Convert";
}
