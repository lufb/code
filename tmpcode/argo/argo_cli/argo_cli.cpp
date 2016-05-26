#include "argo_global.h"
#include "argo_cli.h"
#include "argo_sock.h"
#include "argo_error.h"

#include "./lib/list.h"

#include "MThread.h"


static int argo_init()
{
	int					err;
#ifndef LINUXCODE
	MSocket::initNetWork();
#endif
	err = Global_Log.initLog("./", true, "argo_cli", "log");
	if(err)
		return err;
	Glboal_Mgr.Init();
	INIT_LIST_HEAD(&Global_sc_op_head);
	Global_argo_base.heart_times = 60;
	Global_argo_base.recon_times = 5;
	do{
		Global_Pro_Ctx.pro_buf = NULL;
		Global_Pro_Ctx.pro_buf_size = 0;
		Global_Pro_Ctx.pro_buf_used = 0;
	}while(0);
	
	return Global_Loc_Option.LoadOption();
}

int argo_destroy()
{
	//	TODO
	return 0;
}


int
argo_cli()
{
	int						err;

	err = argo_init();
	if(err){
		Global_Log.writeLog(LOG_TYPE_ERROR, "初始化服务器发生错误[%d][%d]", GET_SYS_ERR(err), GET_USER_ERR(err));
		return err;
	}

	Global_Log.writeLog(LOG_TYPE_INFO, "argo启动成功，程序主子版本号[%u:%u], BDE层主子版本号[%u:%u]", 
		Global_Main_Ver, Global_Child_Ver,
		Global_BDE_Main, Global_BDE_Child);

	return Glboal_Mgr.ArgoFun();
}