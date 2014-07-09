#ifndef __PSVRCOMDEF_H_111XXJJDD
#define __PSVRCOMDEF_H_111XXJJDD

#include <utility>
#include "framecommon/framecommon.h"

using namespace std;
using namespace MYFRAMECOMMON;

//Protocol definition
//const int PROTOCOLVER = 0x35;

struct PPROTOMSGNAMEITEM
{
	int iMsgID;
	char *pMsgName;
};

//Msg ID definition
enum PSrvMsgID
{
	ISRC_ONLINE = 1,
	ISRC_ONLINE_RESP = 2,
	REPORT_RCLIST = 3,
	REPORT_RCLIST_RESP = 4,
	INSERT_RC = 5,
	INSERT_RC_RESP = 6,
	DELETE_RC = 7,
	DELETE_RC_RESP = 8,
	PEER_QUERY = 9,
	PEER_QUERY_RESP = 10,
	INVALID_PEER = 11
};

//global variant declaration
extern PPROTOMSGNAMEITEM g_MsgName[];

//global funtion declaration
extern char *GetMsgNameByID(int msgID);


#endif
