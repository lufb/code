//------------------------------------------------------------------------------------------------------------------------------
//单元名称：INI配置文件单元
//单元描述：封装了INI文件的操作
//创建日期：2007.3.18
//创建人员：卢明远
//修改纪录
//修改日期		修改人员	修改内容和原因
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
	char							szSectionName[32];						//Section的名称
	long							lFirstChildSerial;						//第一个子节点序号
} tagMIniFileSection;
//..............................................................................................................................
typedef struct
{
	char							szNodeName[32];							//Node节点名称
	char							szNodeValue[100];						//Node节点值
	long							lNextNodeSerial;						//下一个Node节点的序号
} tagMIniFileNode;
//..............................................................................................................................
typedef struct
{
	unsigned char					cType;									//类型 0x00 Section的注释 0x01 Node的注释（注意：包括右边和下边的注释）
	long							lSectionNodeSerial;						//Section或Node的序号
	long							lDescribeSerial;						//注释开始的序号
} tagMIniFileDescribe;
//..............................................................................................................................
#pragma pack()
//------------------------------------------------------------------------------------------------------------------------------
//注意：只允许在单线程操作，如果要在多线程下同时使用，请自行加入同步保护
//..............................................................................................................................
#define MINIFILE_DEFALUT_SECTONCOUNT					20					//缺省Section容量
#define MINIFILE_DEFALUT_NODECOUNT						100					//缺省Node容量
#define MINIFILE_DEFALUT_DESCRIBECOUNT					8192				//缺省Node容量
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
	//读入的Node只有名称没有等号和值
	__inline int  inner_readnode2(const char * strIn);
protected:
	__inline int  inner_autospace(MFile * lpFilePtr,const char * strIn);
	__inline int  inner_writefile(void);
public:
	MIniFile(void);
	MIniFile( unsigned long NodeLimit );
	virtual ~MIniFile();
public:
	//打开INI文件，如果打开成功返回>=0，文件不存在返回<0，文件不存在的情况下也可以进行写入操作等
	//nDh = 1读入的node有等号，nDh = 0读放的node没有等号
	int  Open(MString strFileName, int nDh = 1);
	//关闭文件
	void Close(void);
	//写入关闭文件
	int WriteAndClose(void);
public:
	//读取数据
	MString ReadString(MString strSection,MString strNode,MString strDefault);
	int  ReadInteger(MString strSection,MString strNode,int iDefault);
	double ReadDouble(MString strSection,MString strNode,double dDefault);
	bool ReadBool(MString strSection,MString strNode,bool bDefault);
public:
	//写入数据
	int  WriteString(MString strSection,MString strNode,MString strValue,MString strDescribe = "");
	int  WriteInteger(MString strSection,MString strNode,int iValue,MString strDescribe = "");
	int  WriteDouble(MString strSection,MString strNode,double dValue,MString strDescribe = "");
	int  WriteBool(MString strSection,MString strNode,bool bValue,MString strDescribe = "");
public:
	//获取Section数量
	int  GetSectionCount(void);
	//获取Section名称，为""表示失败
	MString GetSectionName(unsigned long lSerial);
	//add by liuqy 20100429
	/***********************************************************************
	 *	函数名：rl_ReadSection
	 *	功能：读取指定应用下的所有节点
	 *	参数：in_strSection			输入-应用名称
	 *		out_ppszNodeNameList	输出-节点名称指针数组
	 *		out_ppszNodeValList		输出-节点值指针数组
	 *		in_lNodeNameValListSize	输入-节点名称和值的数据组大小
	 *	返回：读取数据的个数，0表示无，-1表示错误
	 *
	 ***********************************************************************/
	long rl_ReadSection(MString in_strSection, char ** out_ppszNodeNameList, char ** out_ppszNodeValList, long in_lNodeNameValListSize);

};
//------------------------------------------------------------------------------------------------------------------------------
//注意：该配置文件读取是调用Close才把更新内容写入文件
//------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------
