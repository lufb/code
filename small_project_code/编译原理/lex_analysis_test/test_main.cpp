#include <stdio.h>
#include <WINDOWS.H>
#include <assert.h>
#include "../lex_analysis/scan.h"
/*
	解析类似满足以下格式的一种简单计算器的语言
	1	注释由'{'开头，并由'}'结束，不允许嵌套
	2	保留字仅有"echo"，即打印某个变量，并且，echo 时只能给一个变量，不能跟表达式
	3	特殊符号有 '+','-','*','/',';',其中';'表示一条语句完成
	4	其它，数(一个或更多的数字，数暂时只考虑整形)，标识符(一个或更多的字母)
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
