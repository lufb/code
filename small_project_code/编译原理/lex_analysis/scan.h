#ifndef _MY_SCAN_H_
#define _MY_SCAN_H_





/* DFA 状态 */
typedef enum
{
	START,						/* 起始状态	*/
	INCOMMENT,INID, INNUM,		/* 中间状态	*/
	DONE						/* 结束状态	*/
}dfa_state;

/* 字串类型 */
typedef enum
{
	ENDFILE, E_RROR,			/* 文件结束或有错	*/
//	ECHO,						/* echo,不用单独用类型，用ID表示 */
	ID, NUM,					/* ID/NUM			*/
	COMMENT,					/* 注释				*/
	ASSIGN,						/* =				*/
	PLUS, MINUS, TIMES, OVER,	/*		+ - * /		*/
	L_BRACK, R_BRACK,			/*		(	)		*/
	SENTENCE_END				/*		;			*/
}token_type;

/* 字符类型	*/
typedef enum
{
	TYPE_CHAR,									/* 字母			*/
	TYPE_DIGTAL,								/* 数字			*/
	TYPE_BLANK,									/* 空白字符		*/
	TYPE_L_COMMENT, TYPE_R_COMMENT,				/* 左(右)大括号	*/
	TYPE_L_BRACK, TYPE_R_BRACK,					/* 左(右)小排号	*/
	TYPE_ADD, TYPE_MINUS, TYPE_TIMES, TYPE_OVER,/* 加减乘除	*/
	TYPE_EQ,									/* 赋值符号(=)	*/
	TYPE_SENT_END,								/* 语句结束符	*/
	TYPE_UNKNOW									/* 不认识的字符	*/
}char_type;

/* 字串的最大长度	*/
#define		MAX_ID_LEN				8192

/* 描述串 */
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
