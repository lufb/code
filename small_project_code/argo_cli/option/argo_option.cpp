#include <string.h>
#include "argo_global.h"
#include "argo_option.h"
#include "argo_pro.h"
#include "argo_error.h"
#include "MIniFile.h"
#include "argo_base.h"

MOptin::MOptin()
{
	strcpy(ini_name, "./argo_cli.ini");
}

MOptin::~MOptin()
{

}

int MOptin::LoadOption()
{
	MIniFile			oIniFile;
	int					err;

	if((err = oIniFile.Open(ini_name)) < 0){			
		Global_Log.writeLog(LOG_TYPE_ERROR, "¶ÁÈ¡ÅäÖÃÎÄ¼þ[%s]Ê§°Ü[%d]", ini_name, err);
		return BUILD_ERROR(_OSerrno(), E_LOAD_OP);
	}
	
	strncpy(op_main, oIniFile.ReadString("config", "main_dir", "./").c_str(), sizeof(op_main)-1);
	if(strlen(op_main) > 0 && (op_main[strlen(op_main)-1] == '/' || op_main[strlen(op_main)-1] == '\\'))
		op_main[strlen(op_main)-1] = 0;
	dos2unix_path(op_main, strlen(op_main));
	strncpy(IP, oIniFile.ReadString("config", "ip", "127.0.0.1").c_str(), sizeof(IP)-1);
	port = (unsigned short)(oIniFile.ReadInteger("config", "port", 9999));

	return 0;
}

char*	MOptin::GetMain()
{
	return op_main;
}

char*	MOptin::GetTmpDir()
{
	return op_tmpdir;
}

char*	MOptin::GetIP()
{
	return IP;
}

unsigned short	MOptin::GetPort()
{
	return port;
}