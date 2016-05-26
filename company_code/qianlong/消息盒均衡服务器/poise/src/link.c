#include "link.h"
#include "poise.h"
#include "request.h"

extern SRV_INFO					*gSrvInfo;//��������Ϣ����
extern struct futex_mutex		gLock;

/*************************************************
*	��������do_passive_conn_close
*	��������·�Ͽ�������
*	������
*			@msg_hdr��ƽ̨����������Ϣͷ
*	���أ�0
*
*************************************************/
int do_passive_conn_close(struct msg_header *msg_hdr)
{
	struct _util_ops			*util_ops = &(mbi_sp_interface->util_ops);
	unsigned char				ucType;
	//ȡƽ̨����link_no�е�16λ��Ϊ�����������±�
	unsigned int LinkNo = (msg_hdr->link_no)&0xFFFF;
	
	futex_mutex_lock(&gLock);//����
	ucType = gSrvInfo[LinkNo].m_cType;
	futex_mutex_unlock(&gLock);//����
	//�öϿ��ǿͻ���ȡ���б��Ͽ���������������ô���
	if(ucType != 'U' && ucType != 'P')
	{
		return 0;
	}
	if(ucType == 'P')
	{
		//����ע��
		delAgent(msg_hdr, -1);
		util_ops->write_log(demo_module_no, LOG_TYPE_INFO, LOG_LEVEL_DEVELOPERS, "���������LinkNoΪ%6d ��·�Ͽ�", msg_hdr->link_no);
	}else if(ucType == 'U')
	{
		//����ע��
		delUpdate(msg_hdr, -1);
		util_ops->write_log(demo_module_no, LOG_TYPE_INFO, LOG_LEVEL_DEVELOPERS, "����������LinkNoΪ%6d ��·�Ͽ�", msg_hdr->link_no);
	}

	return 0;
}