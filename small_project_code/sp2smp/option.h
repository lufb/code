#ifndef _YL_OPTION_H_
#define _YL_OPTION_H_

#include <string.h>

/*
	跟配置相关的函数
*/

#define INI_FILE_NAME		".//sp2smg.ini"		/*配置文件名*/
#define USR_PASS_LEN		16					/*用户名与密码最大长度*/

struct option{
	char			sp2smg_usrname[USR_PASS_LEN+1];		/*sp->smg的用户名*/
	char			sp2smp_usrpass[USR_PASS_LEN+1];		/*sp->smg的密码 */
	char			sp_IP[256];							/*sp的地址*/
	unsigned short	sp_port;							/*sp的端口*/
	unsigned int	sp2smg_timeout;						/*sp到smg链路的超时时间(秒)*/

	char			smg2sp_usrname[USR_PASS_LEN+1];		/*smg->sp的用户名*/
	char			smg2sp_usrpass[USR_PASS_LEN+1];		/*smg->sp的密码*/
	unsigned short	smg_bind_port;						/*本地的监听端口*/					
	unsigned int	smg2sp_timeout;						/*smg到sp链路的超时时间(秒)*/

	option(){
		memset(&sp2smg_usrname, 0, sizeof(struct option));
	}

};

int		load_option(void);
void	print_option(struct option &op);








#endif