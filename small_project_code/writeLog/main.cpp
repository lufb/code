#include <stdio.h>
#include "writelog.h"

MWriteLog					Global_Log;

//test	1 fp == NULL fwrite
//test	2 �᲻�������ٴ��ļ�
//test	3 �ļ�����᲻�����
int main()
{
	int					rc;
	
	rc = Global_Log.initLog(
			"E:\\MyDataManager\\code\\�Լ�д��С��Ŀ����\\writeLog",
			true, "GcClient", "log");

	if(rc != 0)
		return rc;


	Global_Log.writeLog(LOG_TYPE_INFO, "%s%d", "this is test", 1);
	Global_Log.writeLog(LOG_TYPE_INFO, "%s%d", "this is test", 2);


	return 0;
}