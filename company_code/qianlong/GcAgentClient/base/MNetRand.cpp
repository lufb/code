#include <time.h>
#include <stdio.h>
#include "MNetRand.h"
#include "MCrc32.h"


/*	
 *	MNetRand	-	���캯������ʼ����������˲����ó�ʼ���������ڴ溯��
 */
MNetRand::MNetRand()
{
	memset(&m_cMemInfo, 0, sizeof(m_cMemInfo));
	memset(m_cMacInfo, 0, sizeof(m_cMacInfo));
	
	srand((unsigned)time(NULL));
	getMem();
	getMac();
}

/*	
 *	~MNetRand	-	��������
 */

MNetRand::~MNetRand()
{

}

/*	
 *	getMem	-	�õ��ڴ���Ϣ
 *
 *	����ֵ��
 *				��
 */
void MNetRand::getMem()
{
	m_cMemInfo.dwLength=sizeof(MEMORYSTATUS);
	::GlobalMemoryStatus(&m_cMemInfo); 
}

/*	
 *	getARand	-	�õ�һ������������Բ��ظ��������
 *
 *	����ֵ��
 *					�����
 */
unsigned long MNetRand::getARand()
{
	MCRC32				crc32;
	unsigned long		ret = rand();	
	time_t				t = (unsigned)time(NULL);
	
	ret = crc32.CheckCode((char*)&t, sizeof(t), ret);
	ret = crc32.CheckCode((char*)&m_cMemInfo, sizeof(m_cMemInfo), ret);
	ret = crc32.CheckCode(m_cMacInfo, sizeof(m_cMacInfo), ret);
	
	return ret;
}

/*	
 *	getMac	-	��NetAPI����ȡ����MAC��ַ
 *
 *	����ֵ��
 *				��
 */
void MNetRand::getMac()     
{     
	//ͨ��WindowsNT/Win2000�����õ�NetApi32.DLL�Ĺ�����ʵ�ֵġ�����ͨ������NCBENUM����,��ȡ������
	//��Ŀ��ÿ���������ڲ����,Ȼ���ÿ��������ŷ���NCBASTAT�����ȡ��MAC��ַ��
	NCB ncb;     //����һ��NCB(������ƿ�)���͵Ľṹ�����ncb
	
	typedef struct _ASTAT_     //�Զ���һ���ṹ��_ASTAT_
	{
		ADAPTER_STATUS   adapt; 
		NAME_BUFFER   NameBuff   [30];     
	}ASTAT, *PASTAT;
	ASTAT Adapter;   
	
	typedef struct _LANA_ENUM     //�Զ���һ���ṹ��_ASTAT_
	{
		UCHAR length; 
		UCHAR lana[MAX_LANA];     //�������MAC��ַ 
	}LANA_ENUM; 
    
	LANA_ENUM lana_enum;   
	
	//   ȡ��������Ϣ�б�     
	UCHAR uRetCode;     
	memset(&ncb, 0, sizeof(ncb));     //���ѿ����ڴ�ռ�ncb ��ֵ����Ϊֵ 0
	memset(&lana_enum, 0, sizeof(lana_enum));     //���һ���ṹ���͵ı���lana_enum����ֵΪ0
	//�Խṹ�����ncb��ֵ
	ncb.ncb_command = NCBENUM;     //ͳ��ϵͳ������������
	ncb.ncb_buffer = (unsigned char *)&lana_enum; //ncb_buffer��Աָ����LANA_ENUM�ṹ���Ļ�����
	ncb.ncb_length = sizeof(LANA_ENUM);   
	//����������NCBENUM����Ի�ȡ��ǰ������������Ϣ�����ж��ٸ�������ÿ�������ı�ţ�MAC��ַ�� 
	uRetCode = Netbios(&ncb); //����netbois(ncb)��ȡ�������к�    
	if(uRetCode != NRC_GOODRET)     
		return;     
	
	//��ÿһ�������������������Ϊ�����ţ���ȡ��MAC��ַ   
	for(int lana=0; lana<lana_enum.length; lana++)     
	{
		ncb.ncb_command = NCBRESET;   //����������NCBRESET������г�ʼ��
		ncb.ncb_lana_num = lana_enum.lana[lana]; 
		uRetCode = Netbios(&ncb);   
	} 
	if(uRetCode != NRC_GOODRET)
		return;       
	//   ׼��ȡ�ýӿڿ���״̬��ȡ��MAC��ַ
	memset(&ncb, 0, sizeof(ncb)); 
	ncb.ncb_command = NCBASTAT;    //����������NCBSTAT�����ȡ������Ϣ
	ncb.ncb_lana_num = lana_enum.lana[0];     //ָ�������ţ��������ָ����һ��������ͨ��Ϊ�������� 
	strcpy((char*)ncb.ncb_callname, "*");     //Զ��ϵͳ����ֵΪ*
	ncb.ncb_buffer = (unsigned char *)&Adapter; //ָ�����ص���Ϣ��ŵı���
	ncb.ncb_length = sizeof(Adapter);
	//���ŷ���NCBASTAT�����Ի�ȡ��������Ϣ
	uRetCode = Netbios(&ncb); 
	//   ȡ����������Ϣ����������������������Ļ������ر�׼��ð�ŷָ���ʽ��   
	if(uRetCode != NRC_GOODRET)   
		return;   
	//������MAC��ַ��ʽת��Ϊ���õ�16������ʽ,������ַ���mac�� 
	sprintf(m_cMacInfo,"%02X-%02X-%02X-%02X-%02X-%02X",     
		Adapter.adapt.adapter_address[0],     
		Adapter.adapt.adapter_address[1],     
		Adapter.adapt.adapter_address[2],     
		Adapter.adapt.adapter_address[3],     
		Adapter.adapt.adapter_address[4],     
		Adapter.adapt.adapter_address[5] 
		);   
}