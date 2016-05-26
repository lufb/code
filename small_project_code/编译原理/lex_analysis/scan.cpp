#include "scan.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <WINDOWS.H>

FILE			*source;
FILE			*dst;
token_descri	g_desc;


int			
init_scan(char	*src_file)
{
	struct stat  buf;
	if(stat(src_file, &buf) < 0){
		fprintf(stderr, "得到文件[%s]的大小失败[%d]\n", src_file, GetLastError());;
		return -1;
	}
	
	if((source = fopen(src_file, "r")) == NULL){
		fprintf(stderr, "打开文件[%s]失败[%d]\n", src_file, GetLastError());
		
		return -1;
	}
	
	dst = stdout;
	
	return 0;
}
void			
destroy_scan(void)
{
	if(source != NULL)
		fclose(source);
}

int
_is_digit(char looked)
{
	return (	looked >= '0' && 
				looked <= '9'	);
}

int
_is_letter(char looked)
{
	return (	(looked >= 'a' && looked <= 'z') ||
				(looked >= 'A' && looked <= 'Z')	);
}

int
_is_blank(int c)
{
	return (	c == '\r'	||
		c == '\n'	||
		c == '\t'	||
		c == ' '	);
}


char_type
_get_char_type(char looked)
{
	if(_is_digit(looked))
		return TYPE_DIGTAL;
	else if(_is_letter(looked))
		return TYPE_CHAR;
	else if(_is_blank(looked))
		return TYPE_BLANK;
	else if(looked == '{')
		return TYPE_L_COMMENT;
	else if(looked == '}')
		return TYPE_R_COMMENT;
	else if(looked == '(')
		return TYPE_L_BRACK;
	else if(looked == ')')
		return TYPE_R_BRACK;
	else if(looked == '+')
		return TYPE_ADD;
	else if(looked == '-')
		return TYPE_MINUS;
	else if(looked == '*')
		return TYPE_TIMES;
	else if(looked == '/')
		return TYPE_OVER;
	else if(looked == '=')
		return TYPE_EQ;
	else if(looked == ';')
		return TYPE_SENT_END;
	else 
		return TYPE_UNKNOW;
}


int
_get_next_char(void)
{
	return fgetc(source);
}

void
_put_char(int c)
{
	ungetc(c, source);
}

int
is_sendtence_end(int c)
{
	return (c == ';');
}

void
_init_token_descri()
{
	memset((void *)&g_desc, 0, sizeof(g_desc));
}

void
_insert_token(int c)
{

	if(g_desc.used_size + 1 >= sizeof(g_desc.token_string)){
		fprintf(stderr, "缓冲区不够了\n");
		return;
	}
	g_desc.token_string[g_desc.used_size++] = (char)c;
}


int
start_status(char c, dfa_state &state, token_type &type)
{
	assert(state == START);
	char_type			e_type = _get_char_type(c);
	int					save = 1;

	switch(e_type)
	{
	case TYPE_CHAR:
		state = INID;
		break;
	case TYPE_DIGTAL:
		state = INNUM;
		break;
	case TYPE_BLANK:
		save = 0;
		break;
	case TYPE_L_COMMENT:
		state = INCOMMENT;
		save = 0;
		break;
	case TYPE_R_COMMENT:
		state = DONE;
		type = E_RROR;
		save = 0;
		break;
	case TYPE_L_BRACK:
		state = DONE;
		type = L_BRACK;
		break;
	case TYPE_R_BRACK:
		state = DONE;
		type = R_BRACK;
		break;
	case TYPE_ADD:
		state = DONE;
		type = PLUS;
		break;
	case TYPE_MINUS:
		state = DONE;
		type = MINUS;
		break;
	case TYPE_TIMES:
		state = DONE;
		type = TIMES;
		break;
	case TYPE_OVER:
		state = DONE;
		type = OVER;
		break;
	case TYPE_EQ:
		state = DONE;
		type = ASSIGN;
		break;
	case TYPE_SENT_END:
		state = DONE;
		type = SENTENCE_END;
		break;
	case TYPE_UNKNOW:
		fprintf(stderr, "UNKOWN CHARATER [%c]\n", c);
		state = DONE;
		type = E_RROR;
		save = 0;
		break;
	default:
		assert(0);
		break;
	}

	return save;
}

int
comment_status(char c, dfa_state &state, token_type &type)
{
	assert(state == INCOMMENT);
	char_type			e_type = _get_char_type(c);
	int					save = 1;

	switch(e_type)
	{
	case TYPE_R_COMMENT:
		state = DONE;
		type = COMMENT;
		save = 0;
		break;

	case TYPE_L_COMMENT:
	case TYPE_CHAR:
	case TYPE_DIGTAL:
	case TYPE_BLANK:
	case TYPE_L_BRACK:
	case TYPE_R_BRACK:
	case TYPE_ADD:
	case TYPE_MINUS:
	case TYPE_TIMES:
	case TYPE_OVER:
	case TYPE_EQ:
	case TYPE_SENT_END:
		break;
	
	default:
		assert(0);
		break;
	}

	return save;
}

int
id_status(char c, dfa_state &state, token_type &type)
{
	assert(state == INID);
	char_type			e_type = _get_char_type(c);
	int					save = 1;
	
	switch(e_type)
	{
	case TYPE_CHAR:
		break;

	case TYPE_L_BRACK:
	case TYPE_R_COMMENT:
	case TYPE_DIGTAL:
	case TYPE_R_BRACK:		
	case TYPE_BLANK:
	case TYPE_L_COMMENT:
	case TYPE_ADD:
	case TYPE_MINUS:
	case TYPE_TIMES:
	case TYPE_OVER:
	case TYPE_EQ:
	case TYPE_SENT_END:
		state = DONE;
		type = ID;
		save = 0;
		_put_char(c);	/* 回退流 */
		break;
		
	case TYPE_UNKNOW:
		fprintf(stderr, "在INID状态下出现非法字符[%c]\n", c);
		state = DONE;
		type = E_RROR;
		save = 0;
		_put_char(c);
		break;
		
	default:
		assert(0);
		break;
	}	

	return save;
}

int
num_status(char c, dfa_state &state, token_type &type)
{
	assert(state == INNUM);
	char_type			e_type = _get_char_type(c);
	int					save = 1;

	switch(e_type)
	{
	case TYPE_DIGTAL:
		break;
	
	case TYPE_BLANK:
		state = DONE;
		type = NUM;
		save = 0;
		break;

	case TYPE_L_BRACK:
	case TYPE_R_COMMENT:
	case TYPE_CHAR:
	case TYPE_EQ:	
	case TYPE_L_COMMENT:
	case TYPE_R_BRACK:
	case TYPE_ADD:
	case TYPE_MINUS:
	case TYPE_TIMES:
	case TYPE_OVER:
	case TYPE_SENT_END:
		state = DONE;
		type = NUM;
		save = 0;
		_put_char(c);	/* 回退流 */
		break;
		
	case TYPE_UNKNOW:
		fprintf(stderr, "在INNUM状态下出现非法字符[%c]\n", c);
		state = DONE;
		type = E_RROR;
		save = 0;
		break;
		
	default:
		assert(0);
		break;
	}	

	return save;
}


token_type			
get_token(void)
{
	int				tmp_char;
	dfa_state		state = START;		/*	置初始状态			*/
	token_type		ret_type = ENDFILE;			/*	返回的token类型		*/
	int				save;
	
	_init_token_descri();
	while(state != DONE){
		tmp_char = _get_next_char();
		
		if(tmp_char == EOF){
			switch(state){
			case START:
				return ENDFILE;
			case INCOMMENT:
				return COMMENT;
			case INID:
				return ID;
			case INNUM:
				return NUM;
			assert(0);
			}
		}
		
		switch (state)
		{
		case START:
			save = start_status(tmp_char, state, ret_type);
			break;
		case INCOMMENT:
			save = comment_status(tmp_char, state, ret_type);
			break;
		case INID:
			save = id_status(tmp_char, state, ret_type);
			break;
		case INNUM:
			save = num_status(tmp_char, state, ret_type);
			break;
		case DONE:
			assert(0);
			break;
		default:
			assert(0);
			break;
		}
		if(save)
			_insert_token(tmp_char);
	}
	return ret_type;
}



























#if 0

//char			*data;
//unsigned int	parse_index = 0;

int			echo_source	= 1;
int			lineno = 0;








void
init_token()
{
	token_index = 0;
	memset(token_string, 0, sizeof(token_string));
}

void
pr_token(char *explain)
{
	fprintf(dst, "%s[%s]\n", explain, token_string);
	init_token();
}

void
insert_token(int c)
{
	if(token_index < sizeof(token_string)-1)
		token_string[token_index++] = (char)c;
}

void
start_state(int c, state_type &state)
{
	assert(state == START);

	if(c == '{'){
		state = INCOMMENT;
	}else if(c == '}'){
		printf("晕，不该这样首先出现}\n");
	}else if(is_digit(c)){
		state = INNUM;
	}else if(is_letter(c)){
		state = INID;
	}else if(is_blank(c)){
		;
	}else if(c == '='){
		printf("赋值[%c]\n", c);
		state = DONE;
	}
}

void
comment_state(int c, state_type &state)
{
	assert(state == INCOMMENT);
	if(c == '{'){
		printf("不应该嵌套注释\n");
		assert(0);
	}else if(c == '}'){
		pr_token("注释");
		state = DONE;	/*状态回归*/
	}

	/* 其它状态不变 */
}

void
id_state(int c, state_type &state)
{
	if(is_sendtence_end(c)){
		pr_token("ID");
		fprintf(dst, "语句结束符[%c]\n", c);
		state = DONE;
	}else if(is_operator(c)){
		pr_token("ID");
		fprintf(dst, "操作符[%c]\n", c);
		state = DONE;
	}else if(is_blank(c)){
		pr_token("ID");
		state = DONE;
	}else if(c == '='){
		pr_token("ID");
		fprintf(dst, "赋值符[%c]\n", c);
		state = DONE;
	}
}

void
num_state(int c, state_type &state)
{
	if(is_sendtence_end(c)){
		pr_token("NUM");
		fprintf(dst, "语句结束符[%c]\n", c);
		state = DONE;
	}else if(is_operator(c)){
		pr_token("NUM");
		fprintf(dst, "操作符[%c]\n", c);
		state = DONE;
	}else if(is_blank(c)){
		pr_token("NUM");
		state = DONE;
	}
}

void
sentence_state(int c, state_type &state)
{

}

void
seq_state(int c, state_type &state)
{
	
}
state_type
get_token(void)
{
	int				next_char;
	state_type		state = START;	/*	初始状态			*/

	while(state != DONE){
		next_char = get_next_char();
		if(next_char == -1)
			return END_FILE;
		if(next_char == 'a')
			int c = 2;

		switch (state)
		{
			case START:
				start_state(next_char, state);
				break;
			case INCOMMENT:
				comment_state(next_char, state);
				break;
			case INNUM:
				num_state(next_char, state);
				break;
			case INID:
				id_state(next_char, state);
				break;
			default:
				assert(0);
		}
		insert_token(next_char);
	}
	
	return state;
}
#endif



