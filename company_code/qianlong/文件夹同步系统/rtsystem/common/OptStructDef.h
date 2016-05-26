#ifndef _OPT_STRUCT_DEF_H_
#define _OPT_STRUCT_DEF_H_

#include "bits/list.h"
#include "rbtree.h"
#include "../../common/OptBaseDef.h"
#include <time.h>


//root���ڵ�
typedef struct{
	//char					m_cName[MAX_PATH];		//��Ŀ¼��
	struct list_head		head;					//��Ŀ¼������ͷ
	rb_root					root;					//�����root�ڵ�
	unsigned int			m_uiTotalDir;			//��Ŀ¼��
	unsigned int			m_uiTotalFile;			//���ļ���
	unsigned long			m_ulDirCrc;				//����Ŀ¼У����
	
	unsigned int			m_uiAddFile;			//�˴������ļ���
	unsigned int			m_uiChangeFile;			//�˴θı��ļ���
	unsigned int			m_uiDelFile;			//�˴�ɾ���ļ���
	unsigned int			m_uiDelDir;				//�˴�ɾ��Ŀ¼��
	unsigned int			m_uiAddDir;				//�˴�����Ŀ¼��
} tagRootNode;

//�洢һ���ļ������Խڵ�
typedef struct {
	time_t					m_ulLastModifyTime;		//�ļ�����޸�ʱ��
	unsigned long			m_ulFileSize;			//�ļ���С
} tagFileAttr;

//�洢һ��Ŀ¼�����Խڵ�
typedef struct {
	unsigned long			m_ulCRC32;				//Ŀ¼У����
	unsigned long			m_ulFileAndDirTotalNum;	//Ŀ¼���ļ���Ŀ¼���ܸ�������������Ŀ¼
} tagDirAttr;

//�洢һ���ļ�����Ŀ¼�����Խڵ�
typedef struct {
	union{
		tagFileAttr			m_sFileAttr;			//�ļ�����
		tagDirAttr			m_sDirAttr;			//Ŀ¼����
	};
} tagNodeAttr;

//�ļ����ڵ�
typedef struct {
	char					m_cName[12];				//�洢�ļ���
	unsigned long			m_ulNext;				//����ļ������ָָ����һ���ָ�㣬����λ�洢���ü���
} tagFileName;

//�����ļ�ϵͳ��������ڵ�
typedef struct {
	struct list_head		m_sBrother;				//��ͬһĿ¼�µĽڵ㣬���ֵܽڵ�
	struct list_head 		m_sChildren;			//������Ŀ¼�ڵ㣬���ӽڵ�
	unsigned long			m_ulParant;				//ָ�򸸽ڵ㣬ָ�����λ���Ա�ʶ���ļ�����Ŀ¼
	struct rb_node			m_sRBNode;				//������ڵ�
	
	tagFileName 			*m_pName;				//ָ���ļ����ڵ�
	tagNodeAttr				m_sAttr;				//�ڵ㣨�ļ����ߣ����Խڵ�
} tagBasicNode;  //�ܹ�44�ֽ�





#endif