#ifndef _local_define_h_
#define _local_define_h_

/* ���µĺ��ڲ���ʱ������Ҫ������ */
#define LOCAL_PORT			8890			/* ���ؼ����Ķ˿�					*/
#define	TIME_OUT			60				/* ��·��ʱʱ��(��)					*/

#define LOGIN_ID			"cli_id"		/* �ͻ���������¼�������˵��û��˺� 7.2.1*/
#define LOGIN_AUTH			"authcator"		/* �ͻ�����֤�룬��������ͻ��˵ĺϷ���7.2.2*/

/* fixme	*/
#define MY_GB2312				0				/* gd2312����						*/
#define MY_UNICODE				8				/* unicode����						*/

#define UP_MSG				"����SMGW��cp����Ϣ"	/* ������cp���͵���Ϣ��	*/
	



/*���µĺ�һ�㲻�ø���*/
#define SELECT_TV_TIMEOUT	1				/* select�����ĳ�ʱʱ��				*/
#define SOCK_SND2RCV_BUFF_SIZE	(1024*64)	/* �첽�׽��ֽ��պͷ��ͻ�������С	*/

#define CLI_LIST_LEN		512				/* sock����ĳ���(��ͬʱ֧�����ӵĿͻ��˵ĸ���)*/

/* ����Э�鲿�ݵĳ���Э����	*/
#define PRO_SUCESS			0				/* �ɹ�			*/
#define LOGIN_ERR_AUTH		21				/* ��֤����		*/

#endif
