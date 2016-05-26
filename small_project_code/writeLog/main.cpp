#include <stdio.h>
#include "writelog.h"

MWriteLog					Global_Log;

//test	1 fp == NULL fwrite
//test	2 会不会重新再创文件
//test	3 文件过大会不会出错
int main()
{
	int					rc;
	
	rc = Global_Log.initLog(
			"E:\\MyDataManager\\code\\自己写的小项目代码\\writeLog",
			true, "GcClient", "log");

	if(rc != 0)
		return rc;


	Global_Log.writeLog(LOG_TYPE_INFO, "%s%d", "this is test", 1);
	Global_Log.writeLog(LOG_TYPE_INFO, "%s%d", "this is test", 2);


	return 0;
}