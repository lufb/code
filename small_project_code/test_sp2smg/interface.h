#ifndef _YL_INTERFACE_H_
#define _YL_INTERFACE_H_
/*
	��������Э�����ݽṹ
*/

#pragma pack(1)

/*���кŶ���*/
struct serial{
	unsigned int						serialnum[3];
};

/*��Ϣͷ����*/
struct msg_head{
	unsigned int						msg_len;	/*��Ϣ�ܳ��ȣ�����ͷ*/
	unsigned int						msg_id;		/*�������*/
	struct serial						msg_serial;	/*��Ϣ���к�*/		
};

/*bind��Ϣ�嶨��*/
struct msg_bind{
	unsigned char							login_type;/*��¼���͡�
											1��SP��SMG���������ӣ����ڷ�������
											2��SMG��SP���������ӣ����ڷ�������
											3��SMG֮�佨�������ӣ�����ת������
											4��SMG��GNS���������ӣ�����·�ɱ�ļ�����ά��
											5��GNS��SMG���������ӣ�����·�ɱ�ĸ���
											6������GNS֮�佨�������ӣ���������·�ɱ��һ����
											11��SP��SMG�Լ�SMG֮�佨���Ĳ������ӣ����ڸ��ٲ���
											����������*/
	char									login_name[16];	/*�������˸��ͻ��˷���ĵ�¼��*/
	char									login_pass[16]; /*�������˺�Login Name��Ӧ������*/
	char									reserv[8];		/*�����ֶ�*/		
};

/* ��Ϣ(������Ϣ)��Ӧ�嶨��*/
struct response{
	unsigned char							res_code;		/*�Ƿ�ɹ�: == 0�ɹ� != 0 ������*/
	char									reserv[8];		/*�����ֶ�*/
};

/*����submit��Ϣ��ṹ*/
struct msg_submit{
	char									sub_sp_num[21];	/*SP�Ľ������*/
	char									sub_cg_mum[21]; /*���Ѻ��룬�ֻ�����ǰ��"86"�����־�����ҽ���Ⱥ���Ҷ��û��շ�ʱΪ�գ����Ϊ�գ����������Ϣ�����ķ�����UserNumber������û�֧�������Ϊȫ���ַ���"000000000000000000000"����ʾ��������Ϣ�����ķ�����SP֧����*/
	unsigned char							sub_usr_count;	/*���ն���Ϣ���ֻ�������ȡֵ��Χ1��100*/
	char									sub_usr_num[21];/*���ոö���Ϣ���ֻ��ţ����ֶ��ظ�UserCountָ���Ĵ������ֻ�����ǰ��"86"�����־*/
	char									sub_corp_id[5]; /*��ҵ���룬ȡֵ��Χ0-99999*/
	char									sub_srv_type[10];/*ҵ����룬��SP����*/
	unsigned char							sub_free_type;	/*�Ʒ�����*/
	char									sub_freevalue[6];/*ȡֵ��Χ0-99999����������Ϣ���շ�ֵ����λΪ�֣���SP����,���ڰ������շѵ��û�����ֵΪ����ѵ�ֵ*/
	char									sub_giv_value[6];/*ȡֵ��Χ0-99999�������û��Ļ��ѣ���λΪ�֣���SP���壬��ָ��SP���û����͹��ʱ�����ͻ���*/
	unsigned char							sub_agent_flag;	/*���շѱ�־��0��Ӧ�գ�1��ʵ��*/
	unsigned char							sub_morelatetomt;/*����MT��Ϣ��ԭ��0-MO�㲥����ĵ�һ��MT��Ϣ��1-MO�㲥����ķǵ�һ��MT��Ϣ��2-��MO�㲥�����MT��Ϣ��3-ϵͳ���������MT��Ϣ��*/
	unsigned char							sub_pri;		/*���ȼ�0-9�ӵ͵��ߣ�Ĭ��Ϊ0*/
	char									sub_expirre_time[16];/*����Ϣ��������ֹʱ�䣬���Ϊ�գ���ʾʹ�ö���Ϣ���ĵ�ȱʡֵ��ʱ������Ϊ16���ַ�����ʽΪ"yymmddhhmmsstnnp" ������"tnnp"ȡ�̶�ֵ"032+"����Ĭ��ϵͳΪ����ʱ��*/
	char									sub_schedule_time[16];/*����Ϣ��ʱ���͵�ʱ�䣬���Ϊ�գ���ʾ���̷��͸ö���Ϣ��ʱ������Ϊ16���ַ�����ʽΪ"yymmddhhmmsstnnp" ������"tnnp"ȡ�̶�ֵ"032+"����Ĭ��ϵͳΪ����ʱ��*/
	unsigned char							sub_report_flag;	/*״̬������
												0-������Ϣֻ��������ʱҪ����״̬����
												1-������Ϣ��������Ƿ�ɹ���Ҫ����״̬����
												2-������Ϣ����Ҫ����״̬����
												3-������Ϣ��Я�����¼Ʒ���Ϣ�����·����û���Ҫ����״̬����
												����-����
												ȱʡ����Ϊ0*/
	unsigned char							sub_tp_pid;			/*GSMЭ������*/
	unsigned char							sub_tp_udhi;		/*GSMЭ������*/
	unsigned char							sub_msg_cod;		/*����Ϣ�ı����ʽ��
												0����ASCII�ַ���
												3��д������
												4�������Ʊ���
												8��UCS2����
												15: GBK����
												�����μ�GSM3.38��4�ڣ�SMS Data Coding Scheme*/
	unsigned char							sub_msg_type;		/*��Ϣ���ͣ�0-����Ϣ��Ϣ ����������*/
	unsigned int							sub_msg_len;		/*����Ϣ����*/
	/*
	char									sub_msg[sub_msg_len];//��Ϣ��
	char									sub_reserver[8];	//����
	*/
};



/*����deliver��Ϣ��*/
struct msg_deliver{
	unsigned char							dlv_usr_number[21];/*���Ͷ���Ϣ���û��ֻ��ţ��ֻ�����ǰ��"86"�����־*/
	unsigned char							dlv_sp_num[21];		/*SP�Ľ������*/
	unsigned char							dlv_tp_pid;			/*GSMЭ�����͡���ϸ������ο�GSM03.40�е�9.2.3.9*/
	unsigned char							dlv_tp_udhi;		/*GSMЭ�����͡���ϸ������ο�GSM03.40�е�9.2.3.23,��ʹ��1λ���Ҷ���*/
	unsigned char							dlv_msg_cod;		/*����Ϣ�ı����ʽ��
												0����ASCII�ַ���
												3��д������
												4�������Ʊ���
												8��UCS2����
												15: GBK����
												�����μ�GSM3.38��4�ڣ�SMS Data Coding Scheme*/
	unsigned int						dlv_msg_len;		/*����Ϣ����*/
	/*
	char								dlv_msg_content[dlv_msg_len];//��Ϣ��
	char								dlv_reserver[8];	//�����ֶ�
	*/
};




/*����report��Ϣ��*/
struct msg_report{
	struct serial							report_serial;	/*���������漰��Submit��deliver��������к�*/		
	unsigned char							report_type;	/*Report��������
											0������ǰһ��Submit�����״̬����
											1������ǰһ��ǰתDeliver�����״̬����*/
	unsigned char							report_usr_num[21];/*���ն���Ϣ���ֻ��ţ��ֻ�����ǰ��"86"�����־*/
	unsigned char							report_state;	/*���������漰�Ķ���Ϣ�ĵ�ǰִ��״̬
											0�����ͳɹ�
											1���ȴ�����
											2������ʧ��*/
	unsigned char							report_errcode;	/*��State=2ʱΪ������ֵ������Ϊ0*/
	unsigned char							report_reserver[8];/*�����ֶ�*/
};


#pragma pack()



/*��Ϣ���Ͷ���*/
enum{
	SGIP_BIND			=	0x1,
	SGIP_BIND_RESP		=	0x80000001,
	SGIP_UNBIND			=	0x2,
	SGIP_UNBIND_RESP	=	0x80000002,
	SGIP_SUBMIT			=	0x3,
	SGIP_SUBMIT_RESP	=	0x80000003,
	SGIP_DELIVER		=	0x4,
	SGIP_DELIVER_RESP	=	0x80000004,
	SGIP_REPORT			=	0x5,
	SGIP_REPORT_RESP	=	0x80000005
};

#endif