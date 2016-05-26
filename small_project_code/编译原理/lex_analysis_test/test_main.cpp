#include <stdio.h>
#include <WINDOWS.H>
#include <assert.h>
#include "../lex_analysis/scan.h"
/*
	���������������¸�ʽ��һ�ּ򵥼�����������
	1	ע����'{'��ͷ������'}'������������Ƕ��
	2	�����ֽ���"echo"������ӡĳ�����������ң�echo ʱֻ�ܸ�һ�����������ܸ����ʽ
	3	��������� '+','-','*','/',';',����';'��ʾһ��������
	4	��������(һ�����������֣�����ʱֻ��������)����ʶ��(һ����������ĸ)
*/

#define SOURCE_FILE		"my_example.calc"
extern token_descri		g_desc;

void
pr_token(token_type type)
{
	switch(type)
	{
	case ENDFILE:
		if(g_desc.used_size == 0)
			fprintf(stdout, "%s\n", "<ENDFILE>");
		else
			fprintf(stdout, "[%s]\t%s\n", g_desc.token_string, "<ENDFILE>");
		break;
	case E_RROR:
		break;
	case ID:
		fprintf(stdout, "[%s]\t%s\n", g_desc.token_string, "<ID>");
		break;
	case NUM:
		fprintf(stdout, "[%s]\t%s\n", g_desc.token_string, "<NUM>");
		break;
	case COMMENT:
		fprintf(stdout, "[%s]\t%s\n", g_desc.token_string, "<COMMENT>");
		break;
	case ASSIGN:
		fprintf(stdout, "[%s]\t%s\n", g_desc.token_string, "<ASSIGN>");
		break;
	case PLUS:
		fprintf(stdout, "[%s]\t%s\n", g_desc.token_string, "<PLUS>");
		break;
	case MINUS:
		fprintf(stdout, "[%s]\t%s\n", g_desc.token_string, "<MINUS>");
		break;
	case TIMES:
		fprintf(stdout, "[%s]\t%s\n", g_desc.token_string, "<TIMES>");
		break;
	case OVER:
		fprintf(stdout, "[%s]\t%s\n", g_desc.token_string, "<OVER>");
		break;
	case L_BRACK:
		fprintf(stdout, "[%s]\t%s\n", g_desc.token_string, "<L_BRACK>");
		break;
	case R_BRACK:
		fprintf(stdout, "[%s]\t%s\n", g_desc.token_string, "<R_BRACK>");
		break;
	case SENTENCE_END:
		fprintf(stdout, "[%s]\t%s\n", g_desc.token_string, "<SENTENCE_END>");
		break;
	default:
		assert(0);
		break;
	}

	fflush(stdout);
}

int
main(int argc, char *argv[])
{
	token_type		type;
	if(init_scan(SOURCE_FILE) != 0)
		return -1;
	
	while(1){
		type = get_token();
		pr_token(type);
		if(type == ENDFILE)
			break;
	}

	destroy_scan();
	return 0;
}
