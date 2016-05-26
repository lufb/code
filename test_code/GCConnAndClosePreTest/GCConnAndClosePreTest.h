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
	//char				useCrc;		//���ü�У���ˣ���ΪУ��ֻ��Ի��ԣ�����ʱ��memcompare�Ϳ����������Ƿ����仯
	char				isEcho;
	unsigned short		repeatTime;
	unsigned int		maxFrameSize;	//add
	char				fileName[256];	//add
}PARAM;			//7	param

int						Init();
void					printParam(PARAM &param);


#endif