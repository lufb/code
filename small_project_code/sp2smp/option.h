#ifndef _YL_OPTION_H_
#define _YL_OPTION_H_

#include <string.h>

/*
	��������صĺ���
*/

#define INI_FILE_NAME		".//sp2smg.ini"		/*�����ļ���*/
#define USR_PASS_LEN		16					/*�û�����������󳤶�*/

struct option{
	char			sp2smg_usrname[USR_PASS_LEN+1];		/*sp->smg���û���*/
	char			sp2smp_usrpass[USR_PASS_LEN+1];		/*sp->smg������ */
	char			sp_IP[256];							/*sp�ĵ�ַ*/
	unsigned short	sp_port;							/*sp�Ķ˿�*/
	unsigned int	sp2smg_timeout;						/*sp��smg��·�ĳ�ʱʱ��(��)*/

	char			smg2sp_usrname[USR_PASS_LEN+1];		/*smg->sp���û���*/
	char			smg2sp_usrpass[USR_PASS_LEN+1];		/*smg->sp������*/
	unsigned short	smg_bind_port;						/*���صļ����˿�*/					
	unsigned int	smg2sp_timeout;						/*smg��sp��·�ĳ�ʱʱ��(��)*/

	option(){
		memset(&sp2smg_usrname, 0, sizeof(struct option));
	}

};

int		load_option(void);
void	print_option(struct option &op);








#endif