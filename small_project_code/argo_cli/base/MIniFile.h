//------------------------------------------------------------------------------------------------------------------------------
//��Ԫ���ƣ�INI�����ļ���Ԫ
//��Ԫ��������װ��INI�ļ��Ĳ���
//�������ڣ�2007.3.18
//������Ա��¬��Զ
//�޸ļ�¼
//�޸�����		�޸���Ա	�޸����ݺ�ԭ��
//
//
//
//
//
//------------------------------------------------------------------------------------------------------------------------------
#ifndef __MEngine_MIniFileH_NEW__
#define __MEngine_MIniFileH_NEW__
//------------------------------------------------------------------------------------------------------------------------------
#include "MErrorCode.h"
#include "MFile.h"
#include "MSelfRiseArray.h"
//------------------------------------------------------------------------------------------------------------------------------
#pragma pack(1)
//..............................................................................................................................
typedef struct
{
	char							szSectionName[32];						//Section������
	long							lFirstChildSerial;						//��һ���ӽڵ����
} tagMIniFileSection;
//..............................................................................................................................
typedef struct
{
	char							szNodeName[32];							//Node�ڵ�����
	char							szNodeValue[100];						//Node�ڵ�ֵ
	long							lNextNodeSerial;						//��һ��Node�ڵ�����
} tagMIniFileNode;
//..............................................................................................................................
typedef struct
{
	unsigned char					cType;									//���� 0x00 Section��ע�� 0x01 Node��ע�ͣ�ע�⣺�����ұߺ��±ߵ�ע�ͣ�
	long							lSectionNodeSerial;						//Section��Node�����
	long							lDescribeSerial;						//ע�Ϳ�ʼ�����
} tagMIniFileDescribe;
//..............................................................................................................................
#pragma pack()
//------------------------------------------------------------------------------------------------------------------------------
//ע�⣺ֻ�����ڵ��̲߳��������Ҫ�ڶ��߳���ͬʱʹ�ã������м���ͬ������
//..............................................................................................................................
#define MINIFILE_DEFALUT_SECTONCOUNT					20					//ȱʡSection����
#define MINIFILE_DEFALUT_NODECOUNT						100					//ȱʡNode����
#define MINIFILE_DEFALUT_DESCRIBECOUNT					8192				//ȱʡNode����
//..............................................................................................................................
class MIniFile
{
protected:
	MSelfRiseArray4096<tagMIniFileSection>				m_mSection;
	MSelfRiseArray8192<tagMIniFileNode>					m_mNode;
	MSelfRiseArray4096<tagMIniFileDescribe>				m_mDescribe;
	MSelfRiseArray4096<char>							m_mDescribeContent;
protected:
	long												m_lCurSection;
	long												m_lCurNode;
protected:
	MString												m_strFileName;
	bool												m_bEditFlag;
protected:
	__inline int  inner_addsection(const char * szName);
	__inline int  inner_addnode(const char * szName,const char * szValue);
	__inline int  inner_addsectiondescribe(const char * szDescribe);
	__inline int  inner_addnodedescribe(const char * szDescribe);
protected:
	__inline int  inner_readsection(const char * strIn);
	__inline int  inner_readnode(const char * strIn);
	//�����Nodeֻ������û�еȺź�ֵ
	__inline int  inner_readnode2(const char * strIn);
protected:
	__inline int  inner_autospace(MFile * lpFilePtr,const char * strIn);
	__inline int  inner_writefile(void);
public:
	MIniFile(void);
	MIniFile( unsigned long NodeLimit );
	virtual ~MIniFile();
public:
	//��INI�ļ�������򿪳ɹ�����>=0���ļ������ڷ���<0���ļ������ڵ������Ҳ���Խ���д�������
	//nDh = 1�����node�еȺţ�nDh = 0���ŵ�nodeû�еȺ�
	int  Open(MString strFileName, int nDh = 1);
	//�ر��ļ�
	void Close(void);
	//д��ر��ļ�
	int WriteAndClose(void);
public:
	//��ȡ����
	MString ReadString(MString strSection,MString strNode,MString strDefault);
	int  ReadInteger(MString strSection,MString strNode,int iDefault);
	double ReadDouble(MString strSection,MString strNode,double dDefault);
	bool ReadBool(MString strSection,MString strNode,bool bDefault);
public:
	//д������
	int  WriteString(MString strSection,MString strNode,MString strValue,MString strDescribe = "");
	int  WriteInteger(MString strSection,MString strNode,int iValue,MString strDescribe = "");
	int  WriteDouble(MString strSection,MString strNode,double dValue,MString strDescribe = "");
	int  WriteBool(MString strSection,MString strNode,bool bValue,MString strDescribe = "");
public:
	//��ȡSection����
	int  GetSectionCount(void);
	//��ȡSection���ƣ�Ϊ""��ʾʧ��
	MString GetSectionName(unsigned long lSerial);
	//add by liuqy 20100429
	/***********************************************************************
	 *	��������rl_ReadSection
	 *	���ܣ���ȡָ��Ӧ���µ����нڵ�
	 *	������in_strSection			����-Ӧ������
	 *		out_ppszNodeNameList	���-�ڵ�����ָ������
	 *		out_ppszNodeValList		���-�ڵ�ֵָ������
	 *		in_lNodeNameValListSize	����-�ڵ����ƺ�ֵ���������С
	 *	���أ���ȡ���ݵĸ�����0��ʾ�ޣ�-1��ʾ����
	 *
	 ***********************************************************************/
	long rl_ReadSection(MString in_strSection, char ** out_ppszNodeNameList, char ** out_ppszNodeValList, long in_lNodeNameValListSize);

};
//------------------------------------------------------------------------------------------------------------------------------
//ע�⣺�������ļ���ȡ�ǵ���Close�ŰѸ�������д���ļ�
//------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------
