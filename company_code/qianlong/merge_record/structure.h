#ifndef _STRUCTURE_H_
#define _STRUCTURE_H_
		
#define				MAX_INT			2360	/* �����ʱ��ֵ���ֵ*/
#define				MAX_REC			40960	/* �����ļ��ϼ�������ÿ���û�������¼��*/
#define				MAX_LINE		40960	/* �������ļ��ϼ�������ÿ���û��ļ�¼��һ�У���һ�е���󻺳�����С*/
#define				HED_WOD			3		/* ��¼ͷ�д��ĵ��ʸ���*/
#define				PRE_FMT			(HED_WOD+2)		/* һ��������¼����С������*/
#define				MAX_SEP			40960	/* һ���У���󵥴ʸ���*/
#define				FMT_CMD			","		/* �ָ���*/

/*����ʱ�����ֵֹ*/
struct data_peer{
	unsigned int			time_beg;
	unsigned int			time_end;
};

/*�����¼�У�ĳһ�û�������ֹʱ��Ĳ���*/
struct my_mixed{
	unsigned int			count;
	data_peer				data[MAX_REC];
	void init(){
		count = 0;
		for(unsigned int i = 0; i < MAX_REC; ++i){
			data[i].time_beg = (unsigned int)-1;
			data[i].time_end = (unsigned int)-1;
		}
	}
};

/*����һ����¼�У����зָ��ĵ���*/
struct my_sep_data{
	unsigned int			count;
	char					*pdata[MAX_SEP];
	void init(){
		count = 0;
		for(unsigned int i = 0; i < MAX_SEP; ++i){
			pdata[i] = NULL;
		}
	}
	
	void pr(){
		for(unsigned int i = 0; i < count; ++i){
			printf("index: %u	%s\n", i, pdata[i]);
		}
	}
};


#endif


