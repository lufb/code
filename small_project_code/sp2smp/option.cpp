#include "option.h"
#include <windows.h>
#include <stdio.h>
#include "error_define.h"
#include "global.h"




/*
	load_option		加载配置文件项到内存
	return
		== 0		成功
		!= 0		失败，见error_define.h
*/
int
load_option(void)
{
#define		SP_SMG		"sp2smg"
#define		SMG_SP		"smg2sp"

#define		USR_NAME	"usr"
#define		USR_PASS	"pass"
#define		IP			"ip"
#define		PORT		"port"
#define		TIME_OUT	"timeout"

	int					err;
	char				tmpbuf[256];

	//sp2smg
	if((err = GetPrivateProfileString(SP_SMG, 
		USR_NAME, "", g_option.sp2smg_usrname, 
		sizeof(g_option.sp2smg_usrname), 
		INI_FILE_NAME)) == 0)
		return BUILD_ERROR(0, E_OPTION);

	if((err = GetPrivateProfileString(SP_SMG, 
		USR_PASS, "", g_option.sp2smp_usrpass, 
		sizeof(g_option.sp2smp_usrpass), 
		INI_FILE_NAME)) == 0)
		return BUILD_ERROR(0, E_OPTION);

	if((err = GetPrivateProfileString(SP_SMG, 
		IP, "", g_option.sp_IP, 
		sizeof(g_option.sp2smg_usrname), 
		INI_FILE_NAME)) == 0)
		return BUILD_ERROR(0, E_OPTION);

	if((err = GetPrivateProfileString(SP_SMG, 
		PORT, "", tmpbuf, 
		sizeof(tmpbuf), 
		INI_FILE_NAME)) == 0)
		return BUILD_ERROR(0, E_OPTION);
	g_option.sp_port = atoi(tmpbuf);

	if((err = GetPrivateProfileString(SP_SMG, 
		TIME_OUT, "", tmpbuf, 
		sizeof(tmpbuf), 
		INI_FILE_NAME)) == 0)
		return BUILD_ERROR(0, E_OPTION);
	g_option.sp2smg_timeout = atoi(tmpbuf);

	//smg2sp
	if((err = GetPrivateProfileString(SMG_SP, 
		USR_NAME, "", g_option.smg2sp_usrname, 
		sizeof(g_option.smg2sp_usrname), 
		INI_FILE_NAME)) == 0)
		return BUILD_ERROR(0, E_OPTION);
	
	if((err = GetPrivateProfileString(SMG_SP, 
		USR_PASS, "", g_option.smg2sp_usrpass, 
		sizeof(g_option.smg2sp_usrpass), 
		INI_FILE_NAME)) == 0)
		return BUILD_ERROR(0, E_OPTION);
	
	
	if((err = GetPrivateProfileString(SMG_SP, 
		PORT, "", tmpbuf, 
		sizeof(tmpbuf), 
		INI_FILE_NAME)) == 0)
		return BUILD_ERROR(0, E_OPTION);
	g_option.smg_bind_port = atoi(tmpbuf);
	
	if((err = GetPrivateProfileString(SMG_SP, 
		TIME_OUT, "", tmpbuf, 
		sizeof(tmpbuf), 
		INI_FILE_NAME)) == 0)
		return BUILD_ERROR(0, E_OPTION);
	g_option.smg2sp_timeout = atoi(tmpbuf);

	return 0;

#undef		SP_SMG	
#undef		SMG_SP		
#undef		USR_NAME
#undef		USR_PASS
#undef		IP
#undef		PORT
#undef		TIME_OUT
}

/*
	print_option	打印配置文件
	
	return
					无
*/
void
print_option(struct option &op)
{
	g_log.writeLog(LOG_TYPE_INFO, "sp->smg的用户名[%s]", op.sp2smg_usrname);
	g_log.writeLog(LOG_TYPE_INFO, "sp->smg的密码[%s]", op.sp2smp_usrpass);
	g_log.writeLog(LOG_TYPE_INFO, "sp的地址[%s]", op.sp_IP);
	g_log.writeLog(LOG_TYPE_INFO, "sp的端口[%u]", op.sp_port);
	g_log.writeLog(LOG_TYPE_INFO, "sp到smg链路的超时时间(秒)[%u]", op.sp2smg_timeout);

	g_log.writeLog(LOG_TYPE_INFO, "smg->sp的用户名[%s]", op.smg2sp_usrname);
	g_log.writeLog(LOG_TYPE_INFO, "smg->sp的密码[%s]", op.smg2sp_usrpass);
	g_log.writeLog(LOG_TYPE_INFO, "本地的监听端口[%u]", op.smg_bind_port);
	g_log.writeLog(LOG_TYPE_INFO, "smg到sp链路的超时时间(秒)[%u]", op.smg2sp_timeout);

}
