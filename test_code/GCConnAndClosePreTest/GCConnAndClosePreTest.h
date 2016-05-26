#ifndef _GC_PRE_TEST_H_
#define _GC_PRE_TEST_H_

#define MAX_IP_LEN			64
//#define	PARAM_COUNT			7
#define SET_RED			setConsoleColor(FOREGROUND_RED);
#define SET_BACK		setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

typedef struct _PARAM
{
	char				DstIP[MAX_IP_LEN];
	unsigned short		DstPort;
	char				GcIP[MAX_IP_LEN];
	unsigned short		GcPort;
	char				useGc;
	//char				useCrc;		//不用加校验了，因为校验只针对回显，回显时按memcompare就可晓得数据是否发生变化
	char				isEcho;
	unsigned short		repeatTime;
	unsigned int		maxFrameSize;	//add
	char				fileName[256];	//add
}PARAM;			//7	param

int						Init();
void					printParam(PARAM &param);


#endif