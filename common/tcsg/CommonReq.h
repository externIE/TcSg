#pragma once

typedef struct _tagSERVERPULSE_INFO{
   	int   nCurrentDate;    // 20150303
	int   nLatestTime;     // 2038秒
	int   nReconnectCount; // 每日统计，次日清0
}SERVERPULSE_INFO, *LPSERVERPULSE_INFO;