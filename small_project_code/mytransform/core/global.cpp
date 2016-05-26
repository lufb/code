#include "global.h"
#include "mlog.h"
#include "error.h"
#include "msocket.h"


MWriteLog				global_log;
MOption					global_option;


/**
 *	mytrans		-		��ת����
 *
 *	@argc	:		[in]		�����������
 *	@argv	:		[in]		�����б�
 *	return
 *		==0			�ɹ�
 *		!=0			ʧ��
 */
int mytrans(int argc, char *argv[])
{
	int						rc;

	if((rc = global_log.initLog("./", true, "mytransform", ".txt")) < 0){
		return BUILD_ERROR(0, E_LOG);
	}
	global_log.writeLog(LOG_TYPE_INFO, "��־ģ�������ɹ�");

	if((rc = MSocket::initNetWork()) != 0){
		return BUILD_ERROR(_OSerrno(), E_NETWORKENV);
	}
	global_log.writeLog(LOG_TYPE_INFO, "��ʼ�����绷���ɹ�");

	if((rc = global_option.init(argc, argv)) != 0)
		return rc;





	return 0;
}