#include "argo_global.h"
#include "argo_cli.h"
#include "argo_sock.h"
#include "argo_error.h"
#include "list.h"
#include "MThread.h"


static int argo_init()
{
	int					err;

	MSocket::initNetWork();
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
	err = Glboal_Mgr.start_mainthread();
	if(err){
		Global_Log.writeLog(LOG_TYPE_ERROR, "主控单元返回错误码[%d][%d]", GET_SYS_ERR(err), GET_USER_ERR(err));
		return err;
	}
	Global_Log.writeLog(LOG_TYPE_INFO, "argo启动成功，主子版本号[%u:%u]", Global_Main_Ver, Global_Child_Ver);
	while(!Glboal_Mgr.is_stoped()){
		MThread::Sleep(500);
	}
	
	Global_Log.writeLog(LOG_TYPE_WARN, "argo工作线程退出");

	return err;
}