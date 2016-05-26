/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		commstruct.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RTϵͳͨѶ�ṹ
	History:		
*********************************************************************************************************/
// #ifndef __COMMSTRUCT_H__
// #define __COMMSTRUCT_H__
// 
// #define	MARKETID_SH				0
// #define	MARKETID_SZ				1
// #define	MARKETID_HK				2
// #define	MARKETID_SHL2			3
// #define MARKETID_UNDEFINE		-1
// 
// #define	ALLFILEMASK				0X1F			// �ֶ������ļ�����
// 
// #define	COMMANDCOUNT			13				// ��Ҫѭ�����͵��������
// 
// #define	COMMAND_STATUS			0X00			// ��ȡ״̬��Ϣ����Э���
// #define	COMMAND_OPTION			0X01			// ��ȡ������Ϣ����Э���
// #define COMMAND_MODIFY			0X02			// ������������Э���
// #define	COMMAND_DOWNDATA		0X03			// ������������Э���
// #define	COMMAND_DOWNINFO		0X04			// ��ȡ�����ļ�����
// #define	COMMAND_GETFAILURE		0X05			// ��ȡ����ʧ���ļ������б�
// #define	COMMAND_CANCELDOWN		0X06			// ��ȡ����ʧ���ļ������б�
// #define	COMMAND_GET_LIST_COUNT	0X07			// ��ȡ�ļ��б�����
// #define	COMMAND_GET_LIST_INFO	0X08			// ��ȡ�б���Ϣ
// #define	COMMAND_SET_UPDATE		0X09			// �����Զ�����
// #define	COMMAND_GET_PROXY		0X0A			// ��ȡ��������
// #define COMMAND_PUT_PROXY		0X0B			// ���´�������
// #define	COMMAND_GET_KINDINFO	0X0C			// ��ȡ��Ʒ��������
// 
// #define	COMMAND_CLOSE			0XFD			// �ر�����
// #define COMMAND_DEFAULT			0XFE			// Ĭ�ϴ�����Ϣ
// #define COMMAND_RESTART			0XFF			// ������������Э���
// 
// #define	DOWN_SUCCESS			100				// ���سɹ�
// 
// #define DOWN_CUT				-1				// �������ʧȥ��ϵ
// #define DOWN_FAILED				-2				// �����ļ�����ʧ��
// #define DOWN_PENDING			-3				// ���ջ��б��ڸ���
// #define DOWN_NOTEXIST			-4				// ������ļ�������
// #define	DOWN_CANCEL				-5				// �û�ȡ������
// #define	DOWN_READY				200				// ׼������
// 
// #define MAX_FILE_NAME_SIZE		32
// 
// #define MAX_FILE_INFO_SIZE		64
// 
// #define FLAG_FIRST_PACKET		1
// #define FLAG_NEED_REQUEST		(1 << 1)
// #define FLAG_LAST_PACKET		(1 << 2)
// 
// #define FLAG_NO_FILE			-1
// #define FLAG_NO_LIST			-2
// #define FLAG_CHANGED			-3
// #define FLAG_CHANGING			-4
// #define FLAG_NEED_TOTAL			-5
// #define FLAG_FILE_ERROR			-6
// #define FLAG_SERVER_ERROR		-7
// #define FLAG_CLIENT_ERROR		-8
// #define FLAG_SERVER_RETRY		-9
// 
// #include "MEngine.hpp"
// 
// enum{
// 	UPDATEMODE_FILE = 0,
// 	UPDATEMODE_RECORD,
// 	UPDATEMODE_DBF,
// 	UPDATEMODE_ZONE
// };
// 
// typedef struct								
// {
// 	int			syncmode;
// 	int			updatemode;
// 	int			cupdatecycle;
// 	int			cupdatetime;
// 	int			supdatecycle;
// 	int			supdatetime;
// 	int			fileheadsize;
// 	int			recordsize;
// 	int			rectimepos;
// 	int			cachesize;
// 	int			market;
// 	MString		info;
// 	MString		SecName;
// 	MString		DirName;
// 	MString		FileName;
// } tagClassInfo;
// 
// #pragma pack(1)
// //ͨ��Qlʱ���ʽ(4�ֽ�)
// typedef struct								
// {
// 	unsigned long		Minute  : 6;			//��[0~59]
// 	unsigned long		Hour    : 5;			//ʱ[0~23]
// 	unsigned long		Day     : 5;			//��[0~31]
// 	unsigned long		Month   : 4;			//��[0~12]
// 	unsigned long		Year    : 12;			//��[0~4095]
// } tagQlDateTime;
// 
// typedef struct 
// {
// 	int		syncmode;
// 	int		updatemode;
// 	int		fileheadsize;
// 	int		recordsize;
// 	int		rectimepos;
// 	int		cupdatecycle;
// 	int		market;
// 	char	info[16];
// } tagCommClassCfg;
// 
// typedef struct
// {
// 	int		nUser;
// } tagCfgRequest;
// 
// typedef struct  
// {
// 	char	cListFlag;
// } tagListFlag;
// 
// typedef struct
// {
// 	int		nMarketCount;
// 	int		nClassCount;
// } tagCfgResponse;
// 
// typedef struct
// {
// 	char				nClassID;
// 	char				nMarketID;
// 	unsigned short		nSerial;				//�ļ����
// 	char				nNeedHead;
// } tagInfoRequst;
// 
// typedef struct
// {
// 	char				nClassID;
// 	char				nMarketID;
// 	unsigned short		nSerial;				//�ļ����
// 	char				sFileName[64];
// 	unsigned long		stUpdateTime;
// 	unsigned long		stCreateTime;
// 	int					nFileSzie;
// 	int					nFlag;					//0bit��ͷ��1bit�м�����跢������2bit������
// 	char				ex[MAX_FILE_INFO_SIZE];
// } tagInfoResponse;
// 
// typedef struct
// {
// 	__int64				stOffsetTime;
// 	int					nOffset;
// 	unsigned long		stUpdateTime;
// 	unsigned long 		nFileSzie;
// 	char				nClassID;
// 	char				nMarketID;
// 	unsigned short		nSerial;				//�ļ����
// } tagFileRequst;
// 
// typedef struct
// {
// 	__int64				stOffsetTime;
// 	int					nOffset;
// 	unsigned long		stUpdateTime;
// 	unsigned long 		nFileSzie;
// 	char				nClassID;
// 	char				nMarketID;
// 	unsigned short		nSerial;				//�ļ����
// 	char				sFileName[64];
// 	__int64				stNextDateTime;
// 	int					nNextOffset;
// 	int					nFlag;					//0bit��ͷ��1bit�м�����跢������2bit������
// } tagFileResponse;
// 
// typedef struct
// {
// 	unsigned long		stDateTime;				//����
// 	char				nClassID;
// 	char				nMarketID;
// 	unsigned short		nSerial;
// 	int					nListCookie;
// } tagListRequst;
// 
// typedef struct
// {
// 	unsigned long		stDateTime;				//����
// 	char				nClassID;
// 	char				nMarketID;
// 	unsigned short		nFileCount;
// 	unsigned short		nNextSerial;
// 	int					nListCookie;
// 	int					nFlag;					//0bit��ͷ��1bit�м�����跢������2bit������
// } tagListResponse;
// 
// typedef struct
// {
// 	char				nClassID;
// 	char				nMarketID;	
// } tagListInfoRequest;
// 
// typedef struct
// {
// 	char				nClassID;
// 	char				nMarketID;	
// 	int					nFileCount;
// 	int					nListCookie;
// 	int					nFileCookie;
// 	tagCommClassCfg		stCfg;
// 	int					nFlag;
// } tagListInfoResponse;
// 
// typedef struct  
// {
// 	unsigned short		nRequestNo;
// 	unsigned short		nFrameNo;
// } tagRecvInfo;
// 
// //for tool
// 
// // ��ȡ״̬����ṹ
// typedef struct  
// {
// 	unsigned char		cProtocol;
// }tagStatusRequest, tagRestartRequest, tagListCountRequest;
// 
// // ��ȡ״̬Ӧ��ṹ
// typedef struct
// {
// 	unsigned char		cProtocol;
// 	char				cCommSerial;				// �������б���ţ�-1��ʾδ���ӣ�
// 	unsigned long		stRunTime;					// ���ջ���Ӫʱ��
// 	unsigned long		nUpFile;					// �Զ������ļ�����
// 	char				cAutoUpdateOn;				// �Զ�����״̬
// }tagStatusResponse;
// 
// // ��ȡ�ļ��б�����
// typedef struct 
// {
// 	unsigned char		cProtocol;
// 	char				cMarketCount;
// 	char				cClassCount;
// 	int					nFlag;
// } tagListCountResponse;
// 
// //���ÿ���б����Ϣ
// typedef struct  
// {
// 	unsigned char		cProtocol;
// 	char				cMarketID;
// 	char				cClassID;
// 	int					nFlag;
// } tagListInfoReq, tagSetUpdateReq, tagSetUpdateRes, tagKindInfoReq;
//  
// typedef struct  
// {
// 	unsigned char		cProtocol;
// 	char				cMarketID;
// 
// 	char				cClassID;
// 	int					nFlag;
// 	char				market;
// 	char				name[16];
// 	char				dir[64];
// 	char				info[16];
// 	char				syncmode;
// 	char				updatemode;
// 	int					timecycle;
// 	char				autoupdate;
// } tagListInfoRes;
// 
// // ��ȡ����Ʒ����������Ϣ
// typedef struct 
// {
// 	unsigned char		cProtocol;
// 	char				cMarketID;	
// 	char				cClassID;			// ���������ã�����Ҳ��
// 	int					nFlag;				// >0�ɹ�
// 	char				KindName[16][10];	// һ���г����16����𣬲���ĺ���Ϊ���ַ�����ÿ���������8���ֽڣ�ʣ�������ֽ�Ϊ0��˳��Ϊkind ID��
// 											// �г��ţ�downdata��downinfo��canceldown��faillistҲ��Ӧ�ı��˽ṹ������kind ID��kindID��0��ʼ��-1��ʾ�������
// } tagKindInfoRes;
// 
// // ��ȡ�����ļ�����ṹ���޸������ļ�Ӧ��ṹ
// typedef struct
// {
// 	unsigned char		cProtocol;
// 	bool				bSuccess;				// �޸ĳɹ���ʧ�ܱ�־��1���ɹ���0��ʧ��
// } tagOptionRequest, tagModifyResponse, tagRestartResponse, tagGetProxyRequest, tagPutProxyResponse;
// 
// typedef struct
// {
// 	unsigned char		cProtocol;
// 	char				cClassID;
// 	char				cKindID;
// 	char				cMarketID;
// } tagCancelDownRequest;
// 
// typedef struct 
// {
// 	unsigned char		cProtocol;
// 	char				cClassID;
// 	char				cMarketID;
// 	char				cKindID;
// 	char				cSuccess;
// } tagCancelDownResponse;
// 
// // ��ȡ�����ļ�Ӧ��ṹ���޸������ļ�����ṹ
// typedef struct
// {
// 	unsigned char		cProtocol;
// 	char				cCount;					// ���÷���������
// 	struct Option
// 	{
// 		char			IP[16];
// 		unsigned short	Port;
// 		char			priority;				// ���������ȼ�
// 	} Option[8];
// 	char				MainPath[64];
// }tagOptionResponse, tagModifyRequest;
// 
// // ������������ṹ
// typedef struct 
// {
// 	unsigned char		cProtocol;
// 	char				cClassID;
// 	char				cMarketID;
// 	char				cKindID;
// 	char				szCode[MAX_FILE_NAME_SIZE];
// 	char				updatemode;
// 	tagQlDateTime		stStartTime;
// 	tagQlDateTime		stEndTime;
// } tagDownDataRequst;
// 
// //nFileMask
// //1<<0: realtime		// ���վ���
// //1<<1: 1 min			// һ������
// //1<<2: 15 min			// ʮ�������
// //1<<3: day				// ����
// //1<<4: mon				// ����
// //
// //1<<16, 1<<17: downrage ���ط�Χ��0��ȫ�����أ�1���������أ�2���������أ�
// 
// typedef struct 
// {
// 	unsigned char		cProtocol;
// 	char				cClassID;
// 	char				cMarketID;
// 	char				cKindID;
// 	char				szCode[MAX_FILE_NAME_SIZE];
// 	char				updatemode;
// 	tagQlDateTime		stStartTime;
// 	tagQlDateTime		stEndTime;
// 	int					nRet;
// } tagDownDataResponse;
// 
// typedef struct 
// {
// 	unsigned char		cProtocol;
// 	char				cClassID;
// 	char				cMarketID;
// 	char				cKindID;
// 	char				szCode[MAX_FILE_NAME_SIZE];
// } tagDownInfoRequst;
// 
// typedef struct 
// {
// 	unsigned char		cProtocol;
// 	char				cClassID;
// 	char				cMarketID;
// 	char				cKindID;
// 	char				code[MAX_FILE_NAME_SIZE];
// 	unsigned int		nTotal;				// �ļ��ܸ���
// 	unsigned int		nSuccCount;			// �Ѿ������ļ�����
// 	unsigned int		nFailure;			// ����ʧ�ܸ���
// 	char				cPercent;			// -1 ʧ��, -2��ɣ�-3δ����-4û�����ݲ����ڡ�
// } tagDownInfoResponse;
// 
// // ����ʧ�ܴ����б�ṹ
// typedef struct  
// {
// 	unsigned char		cProtocol;	
// 	unsigned long		nReqNo;
// 	char				cClassID;
// 	char				cMarketID;
// 	unsigned long		nFileMask;
// 	unsigned short		nListSerial;		// ������ʼ�б����
// 	unsigned short		usSerial;			// ������ʼ���
// } tagFailCodeRequest;
// 
// // ʧ�ܴ����б���Ӧ�ṹ
// typedef struct  
// {
// 	unsigned char		cProtocol;	
// 	unsigned long		nReqNo;
// 	char				cClassID;
// 	char				cMarketID;
// 	unsigned long		nFileMask;
// 	char				cCount;				// �������
// 	unsigned short		nListSerial;		// ��һ���б���ţ�
// 	int					usSerial;			// ���һ���������һ�����, -2��ɣ�-3δ����-4û�����ݲ����ڡ�
// 	int					nListCookie;
// } tagFailCodeResponse;
// 
// // ������������ýṹ
// typedef struct 
// {
// 	unsigned char		cProtocol;
// 	char				type;				// -1: �޴���0��socket4��1��socket5��2��http��3��usb��4��gc����ɫͨ����
// 	char				ip[16];
// 	unsigned short		port;
// 	char				user[64];
// 	char				pwd[64];
// } tagPutProxyRequest, tagGetProxyResponse;
// 
// #pragma pack()
// 
// #endif //__COMMSTRUCT_H__
/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		commstruct.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RTϵͳͨѶ�ṹ
	History:		
*********************************************************************************************************/
#ifndef __COMMSTRUCT_H__
#define __COMMSTRUCT_H__

#define	MARKETID_SH				0
#define	MARKETID_SZ				1
#define	MARKETID_HK				2
#define	MARKETID_SHL2			3
#define MARKETID_UNDEFINE		-1

#define	ALLFILEMASK				0X1F			// �ֶ������ļ�����

#define	COMMANDCOUNT			13				// ��Ҫѭ�����͵��������

#define	COMMAND_STATUS			0X00			// ��ȡ״̬��Ϣ����Э���
#define	COMMAND_OPTION			0X01			// ��ȡ������Ϣ����Э���
#define COMMAND_MODIFY			0X02			// ������������Э���
#define	COMMAND_DOWNDATA		0X03			// ������������Э���
#define	COMMAND_DOWNINFO		0X04			// ��ȡ�����ļ�����
#define	COMMAND_GETFAILURE		0X05			// ��ȡ����ʧ���ļ������б�
#define	COMMAND_CANCELDOWN		0X06			// ��ȡ����ʧ���ļ������б�
#define	COMMAND_GET_LIST_COUNT	0X07			// ��ȡ�ļ��б�����
#define	COMMAND_GET_LIST_INFO	0X08			// ��ȡ�б���Ϣ
#define	COMMAND_SET_UPDATE		0X09			// �����Զ�����
#define	COMMAND_GET_PROXY		0X0A			// ��ȡ��������
#define COMMAND_PUT_PROXY		0X0B			// ���´�������
#define	COMMAND_GET_KINDINFO	0X0C			// ��ȡ��Ʒ������Ϣ

#define	COMMAND_CLOSE			0XFD			// �ر�����
#define COMMAND_DEFAULT			0XFE			// Ĭ�ϴ�����Ϣ
#define COMMAND_RESTART			0XFF			// ������������Э���

#define	DOWN_SUCCESS			100				// ���سɹ�

#define DOWN_CUT				-1				// �������ʧȥ��ϵ
#define DOWN_FAILED				-2				// �����ļ�����ʧ��
#define DOWN_PENDING			-3				// ���ջ��б��ڸ���
#define DOWN_NOTEXIST			-4				// ������ļ�������
#define	DOWN_CANCEL				-5				// �û�ȡ������
#define	DOWN_READY				127				// ׼������

//  �����ļ�����״̬��  ����״̬��0��δ���أ�1���������أ�2��������ϣ�-1������ʧ�ܣ�-2��ȡ������
#define DOWNSTATE_CANCEL		-2				// ȡ������
#define	DOWNSTATE_FAILED		-1				// �ļ�����ʧ��
#define	DOWNSTATE_NONE			0				// δ����
#define DOWNSTATE_LOADING		1				// ��������
#define	DOWNSTATE_FINISH		2				// �������


#define MAX_FILE_NAME_SIZE		32

#define MAX_FILE_INFO_SIZE		64

#define FLAG_FIRST_PACKET		1
#define FLAG_NEED_REQUEST		(1 << 1)
#define FLAG_LAST_PACKET		(1 << 2)

#define FLAG_NO_FILE			-1
#define FLAG_NO_LIST			-2
#define FLAG_CHANGED			-3
#define FLAG_CHANGING			-4
#define FLAG_NEED_TOTAL			-5
#define FLAG_FILE_ERROR			-6
#define FLAG_SERVER_ERROR		-7
#define FLAG_CLIENT_ERROR		-8
#define FLAG_SERVER_RETRY		-9

#include "MEngine.hpp"

enum{
	UPDATEMODE_FILE = 0,
	UPDATEMODE_RECORD,
	UPDATEMODE_DBF,
	UPDATEMODE_ZONE
};

typedef struct								
{
	int			syncmode;
	int			updatemode;
	int			cupdatecycle;
	int			cupdatetime;
	int			supdatecycle;
	int			supdatetime;
	int			fileheadsize;
	int			recordsize;
	int			rectimepos;
	int			cachesize;
	int			market;
	MString		info;
	MString		SecName;
	MString		DirName;
	MString		FileName;
	MString		SrcDir;
} tagClassInfo;

//////////////////////////////LUFUBO
typedef struct								
{
	MString		path;
	MString		srcpath;
	MString		srvmainpath;
	MString		name;
	MString		info;
	MString		sectionName;//Directory0
	unsigned int supdatecycle;
	unsigned int cupdatecycle;
	char		syncmode;
	char		updatemode;
	char		usefilecrc32;
	//MString		SecName;
} tagDirTranCfg;
//////////////////////////////LUFUBO

#pragma pack(1)
//��Ԫ��Ϣ
typedef struct
{
	unsigned short				wUnitSerial;			//��Ԫ��ţ��ý��̵ĵڼ�����Ԫ����0��ʼ��
	unsigned short				wUnitType;				//��Ԫ���ͣ�������±���Ԫ���ͱ���
	unsigned short				wPareSerial;			//����Ԫ��
	char						strUnitName[32];		//��Ԫ���ƣ��������ã�
	char						strDescription[40];		//��Ԫ�������������ã�
} tagSmComm_UnitInfo;

//ͨ��Qlʱ���ʽ(4�ֽ�)
typedef struct								
{
	unsigned long		Minute  : 6;			//��[0~59]
	unsigned long		Hour    : 5;			//ʱ[0~23]
	unsigned long		Day     : 5;			//��[0~31]
	unsigned long		Month   : 4;			//��[0~12]
	unsigned long		Year    : 12;			//��[0~4095]
} tagQlDateTime;

typedef struct 
{
	int		syncmode;
	int		updatemode;
	int		fileheadsize;
	int		recordsize;
	int		rectimepos;
	int		cupdatecycle;
	int		market;
	char	info[16];
} tagCommClassCfg;

typedef struct
{
	int		nUser;
} tagCfgRequest;

typedef struct  
{
	char	cListFlag;
} tagListFlag;

typedef struct
{
	int		nMarketCount;
	int		nClassCount;
} tagCfgResponse;

typedef struct
{
	char				nClassID;
	char				nMarketID;
	unsigned short		nSerial;				//�ļ����
	char				nNeedHead;
} tagInfoRequst;

typedef struct
{
	char				nClassID;
	char				nMarketID;
	unsigned short		nSerial;				//�ļ����
	char				sFileName[64];
	unsigned long		stUpdateTime;
	unsigned long		stCreateTime;
	int					nFileSzie;
	int					nFlag;					//0bit��ͷ��1bit�м�����跢������2bit������
	char				ex[MAX_FILE_INFO_SIZE];
} tagInfoResponse;

typedef struct
{
	__int64				stOffsetTime;
	int					nOffset;
	unsigned long		stUpdateTime;
	unsigned long 		nFileSzie;
	char				nClassID;
	char				nMarketID;
	unsigned short		nSerial;				//�ļ����
} tagFileRequst;

typedef struct
{
	__int64				stOffsetTime;
	int					nOffset;
	unsigned long		stUpdateTime;
	unsigned long 		nFileSzie;
	char				nClassID;
	char				nMarketID;
	unsigned short		nSerial;		//�ļ����
	char				sFileName[64];
	__int64				stNextDateTime;
	int					nNextOffset;
	int					nFlag;			//0bit��ͷ��1bit�м�����跢������2bit������
} tagFileResponse;

typedef struct
{
	unsigned long		stDateTime;		//����
	char				nClassID;
	char				nMarketID;
	unsigned short		nSerial;
	int					nListCookie;
} tagListRequst;

typedef struct
{
	unsigned long		stDateTime;		//����
	char				nClassID;
	char				nMarketID;
	unsigned short		nFileCount;
	unsigned short		nNextSerial;
	int					nListCookie;
	int					nFlag;			//0bit��ͷ��1bit�м�����跢������2bit������
} tagListResponse;

typedef struct
{
	char				nClassID;
	char				nMarketID;	
} tagListInfoRequest;

typedef struct
{
	char				nClassID;
	char				nMarketID;	
	int					nFileCount;
	int					nListCookie;
	int					nFileCookie;
	tagCommClassCfg		stCfg;
	int					nFlag;
} tagListInfoResponse;

typedef struct  
{
	unsigned short		nRequestNo;
	unsigned short		nFrameNo;
} tagRecvInfo;

///////////////////////////////////////////////////////////////////////////////////////////////////
//for tool

// ��ȡ״̬����������ͻ�ȡ�ļ��б��������ṹ
typedef struct  
{
	unsigned char		cProtocol;
}tagStatusReq, tagRestartReq, tagListCountReq;

// ��ȡ״̬Ӧ��ṹ
typedef struct
{
	unsigned char		cProtocol;
	char				cCommSerial;				// �������б���ţ�-1��ʾδ���ӣ�
	unsigned long		stRunTime;					// ���ջ���Ӫʱ��
	unsigned long		nUpFile;					// �Զ������ļ�����
	char				cAutoUpdateOn;
}tagStatusRes;

// ��ȡ�ļ��б�����
typedef struct 
{
	unsigned char		cProtocol;
	char				cMarketCount;
	char				cClassCount;
	int					nFlag;
} tagListCountRes;

//���ÿ���б����Ϣ
typedef struct  
{
	unsigned char		cProtocol;
	char				cMarketID;
	char				cClassID;
	int					nFlag;
} tagListInfoReq, tagSetUpdateReq, tagSetUpdateRes, tagKindReq;
 
typedef struct  
{
	unsigned char		cProtocol;
	char				cMarketID;
	char				cClassID;
	int					nFlag;
	char				market;
	char				name[16];
	char				dir[64];
	char				info[16];
	char				syncmode;
	char				updatemode;
	int					timecycle;
	char				autoupdate;
} tagListInfoRes;

typedef struct 
{
	unsigned char		cProtocol;
	char				cMarketID;	
	char				cClassID;			//���������ã�����Ҳ��
	int					nFlag;				//>0�ɹ�
	char				KindName[16][10];	//һ���г����16����𣬲���ĺ���Ϊ���ַ�����ÿ���������8���ֽڣ�ʣ�������ֽ�Ϊ0��˳��Ϊkind ID��//�г��ţ�downdata��downinfo��canceldown��faillistҲ��Ӧ�ı��˽ṹ������kind ID��kindID��0��ʼ��-1��ʾ�������
} tagKindRes;

// ��ȡ�����ļ�����ṹ���޸������ļ�Ӧ��ṹ
typedef struct
{
	unsigned char		cProtocol;
	bool				bSuccess;				// �޸ĳɹ���ʧ�ܱ�־��1���ɹ���0��ʧ��
}tagOptionReq, tagModifyRes, tagRestartRes, tagGetProxyReq, tagPutProxyRes;

typedef struct 
{
	unsigned char		cProtocol;
	char				cClassID;
	char				cMarketID;
	char				cKindID;
} tagCancelDownReq;

typedef struct 
{
	unsigned char		cProtocol;
	char				cClassID;
	char				cMarketID;
	char				cKindID;
	char				cSuccess;
} tagCancelDownRes;

// ��ȡ�����ļ�Ӧ��ṹ���޸������ļ�����ṹ
typedef struct
{
	unsigned char		cProtocol;
	char				cCount;					// ���÷���������
	struct Option
	{
		char			IP[16];
		unsigned short	Port;
		char			priority;				// ���������ȼ�
	} Option[8];
	char				MainPath[64];
}tagOptionRes, tagModifyReq;

// ������������ṹ
typedef struct 
{
	unsigned char	cProtocol;
	char			cClassID;
	char			cMarketID;
	char			cKindID;			
	char			szCode[MAX_FILE_NAME_SIZE];
	char			updatemode;
	tagQlDateTime	stStartTime;
	tagQlDateTime	stEndTime;
} tagDownDataReq;

typedef struct 
{
	unsigned char	cProtocol;
	char			cClassID;
	char			cMarketID;
	char			cKindID;
	char			szCode[MAX_FILE_NAME_SIZE];
	char			updatemode;
	tagQlDateTime	stStartTime;
	tagQlDateTime	stEndTime;
	int				nRet;
} tagDownDataRes;

typedef struct 
{
	unsigned char	cProtocol;
	char			cClassID;
	char			cMarketID;
	char			cKindID;
	char			szCode[MAX_FILE_NAME_SIZE];
} tagDownInfoReq;

typedef struct 
{
	unsigned char	cProtocol;
	char			cClassID;
	char			cMarketID;
	char			cKindID;
	char			code[MAX_FILE_NAME_SIZE];
	unsigned int	nTotal;				// �ļ��ܸ���
	unsigned int	nSuccCount;			// �Ѿ������ļ�����
	unsigned int	nFailure;			// ����ʧ�ܸ���
	char			cPercent;			// -1 ʧ��, -2��ɣ�-3δ����-4û�����ݲ����ڡ�
} tagDownInfoRes;

// ����ʧ�ܴ����б�ṹ
typedef struct  
{
	unsigned char	cProtocol;	
	unsigned long	nReqNo;
	char			cClassID;
	char			cMarketID;
	char			cKindID;
	unsigned long	nFileMask;
	unsigned short  nListSerial;		// ������ʼ�б����
	unsigned short	usSerial;			// ������ʼ���
} tagFailCodeReq;

// ʧ�ܴ����б���Ӧ�ṹ
typedef struct  
{
	unsigned char		cProtocol;	
	unsigned long		nReqNo;
	char				cClassID;
	char				cMarketID;
	char				cKindID;
	unsigned long		nFileMask;
	char				cCount;				// �������
	unsigned short		nListSerial;		// ��һ���б���ţ�
	int					usSerial;			// ���һ���������һ�����, -2��ɣ�-3δ����-4û�����ݲ����ڡ�
	int					nListCookie;
} tagFailCodeRes;

typedef struct 
{
	unsigned char		cProtocol;
	char				type;	//0socket4,1socket5,2http,3usb,4gc����ɫͨ����
	char				ip[16];
	unsigned short		port;
	char				user[64];
	char				pwd[64];
} tagPutProxyReq, tagGetProxyRes;

// ����ṹ������
typedef union _tag_Request
{
	tagStatusReq		StatusReq;				// �����ȡ״̬
	tagRestartReq		RestartReq;				// ��������
	tagListCountReq		ListCountReq;			// �����ļ��б��г������͸���
	tagListInfoReq		ListInfoReq;			// �����ļ��б���Ϣ
	tagSetUpdateReq		SetUpdateReq;			// ���������Զ�����
	tagKindReq			KindReq;				// ������Ʒ����
	tagOptionReq		OptionReq;				// ����������Ϣ
	tagGetProxyReq		GetProxyReq;			// ���������Ϣ
	tagCancelDownReq	CancelDownReq;			// ����ȡ������
	tagModifyReq		ModifyReq;				// �����������
	tagDownDataReq		DownDataReq;			// ������������
	tagDownInfoReq		DownInfoReq;			// �������ؽ�����Ϣ
	tagFailCodeReq		FailCodeReq;			// ��������ʧ���ļ�����
	tagPutProxyReq		PutProxyReq;			// ���ʹ�������
} tag_Request;
#pragma pack()

#endif //__COMMSTRUCT_H__
