#pragma once

typedef struct _tagSERVERPULSE_INFO{
   	int   nCurrentDate;    // 20150303
	int   nLatestTime;     // 2038��
	int   nReconnectCount; // ÿ��ͳ�ƣ�������0
}SERVERPULSE_INFO, *LPSERVERPULSE_INFO;