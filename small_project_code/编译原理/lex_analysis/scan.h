#ifndef _MY_SCAN_H_
#define _MY_SCAN_H_





/* DFA ״̬ */
typedef enum
{
	START,						/* ��ʼ״̬	*/
	INCOMMENT,INID, INNUM,		/* �м�״̬	*/
	DONE						/* ����״̬	*/
}dfa_state;

/* �ִ����� */
typedef enum
{
	ENDFILE, E_RROR,			/* �ļ��������д�	*/
//	ECHO,						/* echo,���õ��������ͣ���ID��ʾ */
	ID, NUM,					/* ID/NUM			*/
	COMMENT,					/* ע��				*/
	ASSIGN,						/* =				*/
	PLUS, MINUS, TIMES, OVER,	/*		+ - * /		*/
	L_BRACK, R_BRACK,			/*		(	)		*/
	SENTENCE_END				/*		;			*/
}token_type;

/* �ַ�����	*/
typedef enum
{
	TYPE_CHAR,									/* ��ĸ			*/
	TYPE_DIGTAL,								/* ����			*/
	TYPE_BLANK,									/* �հ��ַ�		*/
	TYPE_L_COMMENT, TYPE_R_COMMENT,				/* ��(��)������	*/
	TYPE_L_BRACK, TYPE_R_BRACK,					/* ��(��)С�ź�	*/
	TYPE_ADD, TYPE_MINUS, TYPE_TIMES, TYPE_OVER,/* �Ӽ��˳�	*/
	TYPE_EQ,									/* ��ֵ����(=)	*/
	TYPE_SENT_END,								/* ��������	*/
	TYPE_UNKNOW									/* ����ʶ���ַ�	*/
}char_type;

/* �ִ�����󳤶�	*/
#define		MAX_ID_LEN				8192

/* ������ */
typedef struct  
{
	char			token_string[MAX_ID_LEN+1];
	unsigned int	used_size;
}token_descri;

extern token_descri		g_desc;


int					init_scan(char	*src);
void				destroy_scan();
token_type			get_token(void);


#endif
