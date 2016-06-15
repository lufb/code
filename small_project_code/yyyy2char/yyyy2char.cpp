#include "yyyy2char.h"
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#define		PRE_FIX			'<'				/*	需要展开的开始字符	*/
#define		END_FIX			'>'				/*	需要展开的结束字符	*/
#define		MAX_STACK_LEN	32				/*	栈的最大深度		*/
#define		MY_MIN(a, b)	((a) < (b) ? (a) : (b))
#define		ERR_LINE		((-1)*(__LINE__))

typedef struct _my_stack{
	char			data[MAX_STACK_LEN+1];	/*	栈数据				*/
	size_t			depth;					/*	栈深度				*/
	
	void init(){
		depth		= 0;
	}
	
	int	is_empty(){
		return depth == 0 ? 1 : 0;
	}
	
	void push(char c){
		if(depth + 1 < MAX_STACK_LEN)
			data[depth++] = c;
	}
	
	void pop_all(char *out, size_t out_size){
		memset(out, 0, out_size);
		strncpy(out, data, MY_MIN(depth,MY_MIN(out_size, MAX_STACK_LEN)));
		init();
	}
}my_stack;

static my_stack				st;

static int  GetYear(time_t	tm)
{
	#ifndef LINUXCODE
		
		struct tm				*	lptemptime;
		
		if ( (lptemptime = localtime(&tm)) == NULL )
		{
			assert(0);
			return -1;
		}
		
		return(lptemptime->tm_year+1900);
	#else
		
		struct tm					sttemptime;
		
		if ( localtime_r(&tm,&sttemptime) == NULL )
		{
			assert(0);
			return -1;
		}
		
		return(sttemptime.tm_year+1900);
	#endif
}
//..............................................................................................................................
static int  GetMonth(time_t	tm)
{
	#ifndef LINUXCODE
		
		struct tm				*	lptemptime;
		
		if ( (lptemptime = localtime(&tm)) == NULL )
		{
			assert(0);
			return -1;
		}
		
		return(lptemptime->tm_mon+1);
	#else
		
		struct tm					sttemptime;
		
		if ( localtime_r(&tm,&sttemptime) == NULL )
		{
			assert(0);
			return -1;
		}
		
		return(sttemptime.tm_mon+1);
	#endif
}
//..............................................................................................................................
static int  GetDay(time_t	tm)
{
	#ifndef LINUXCODE
		
		struct tm				*	lptemptime;
		
		if ( (lptemptime = localtime(&tm)) == NULL )
		{
			assert(0);
			return -1;
		}
		
		return(lptemptime->tm_mday);
	#else
		
		struct tm					sttemptime;
		
		if ( localtime_r(&tm,&sttemptime) == NULL )
		{
			assert(0);
			return -1;
		}
		
		return(sttemptime.tm_mday);
	#endif
}
//..............................................................................................................................
static int  GetHour(time_t	tm)
{
	#ifndef LINUXCODE
		
		struct tm				*	lptemptime;
		
		if ( (lptemptime = localtime(&tm)) == NULL )
		{
			assert(0);
			return -1;
		}
		
		return(lptemptime->tm_hour);
	#else
		
		struct tm					sttemptime;
		
		if ( localtime_r(&tm,&sttemptime) == NULL )
		{
			assert(0);
			return -1;
		}
		
		return(sttemptime.tm_hour);
	#endif
}
//..............................................................................................................................
static int  GetMinute(time_t	tm)
{
	#ifndef LINUXCODE
		
		struct tm				*	lptemptime;
		
		if ( (lptemptime = localtime(&tm)) == NULL )
		{
			assert(0);
			return -1;
		}
		
		return(lptemptime->tm_min);
	#else
		
		struct tm					sttemptime;
		
		if ( localtime_r(&tm,&sttemptime) == NULL )
		{
			assert(0);
			return -1;
		}
		
		return(sttemptime.tm_min);
	#endif
}
//..............................................................................................................................
static int  GetSecond(time_t	tm)
{
	#ifndef LINUXCODE
		
		struct tm				*	lptemptime;
		
		if ( (lptemptime = localtime(&tm)) == NULL )
		{
			assert(0);
			return -1;
		}
		
		return(lptemptime->tm_sec);
	#else
		
		struct tm					sttemptime;
		
		if ( localtime_r(&tm,&sttemptime) == NULL )
		{
			assert(0);
			return -1;
		}
		
		return(sttemptime.tm_sec);
	#endif
}


static int char_is_correct(char c)
{
	char			cor_char[] = {'y','m','d','h','M','s'};
	unsigned int	i;
	unsigned int	len = sizeof(cor_char)/sizeof(cor_char[0]);
	
	for(i = 0; i < len; ++i)
		if(c == cor_char[i])
			return 1;

	return 0;
}


static int do_parase(char *data, time_t tm)
{
	int					i,len;

	if(data[0] != '<')
		return ERR_LINE;

	memmove(data, data+1, strlen(data));

	len = strlen(data);
	for(i = 0; i < len; ++i){
		if(!char_is_correct(data[i])){
			return ERR_LINE;
		}
	}
	
	switch(data[0]){
	case 'y':
		if(len == 4)
			sprintf(data, "%04d", GetYear(tm));
		else if(len == 2)
			sprintf(data, "%02d", GetYear(tm)%100);
		else
			return ERR_LINE;
		break;
	case 'm':
		if(len == 2)
			sprintf(data, "%02d", GetMonth(tm));
		else
			return ERR_LINE;
		break;
	case 'd':
		if(len == 2)
			sprintf(data, "%02d", GetDay(tm));
		else
			return ERR_LINE;
		break;
	case 'h':
		if(len == 2)
			sprintf(data, "%02d", GetHour(tm));
		else
			return ERR_LINE;
		break;
	case 'M':
		if(len == 2)
			sprintf(data, "%02d", GetMinute(tm));
		else
			return ERR_LINE;
		break;
	case 's':
		if(len == 2)
			sprintf(data, "%02d", GetSecond(tm));
		else
			return ERR_LINE;
		break;
	default:
		return ERR_LINE;
	}

	return 0;
}

int	yyyy2char(const char	*input,			/*	待转换的字符串如：csi-<YYYY>-<MM>-<DD>.txt	*/
			  time_t		tm,				/*	时间										*/
			  char			*out,			/*	转换结果存储位置							*/
			  size_t		out_size		/*	存储结果长度								*/
			  )
{
	unsigned int			i;
	int						err = 0;
	char					tmp_buf[MAX_STACK_LEN+1];
	int						is_in_parase = 0;

	if(strlen(input) > out_size)
		return -1;
	
	tmp_buf[MAX_STACK_LEN] = 0;
	memset(out, 0, out_size);
	st.init();
	for(i = 0; ; ++i){
		switch(input[i]){
		case 0:
			if(is_in_parase == 1)
				return ERR_LINE;
			if(!st.is_empty())
				return ERR_LINE;
			return 0;
		case PRE_FIX:
			if(!st.is_empty())
				return ERR_LINE;
			if(is_in_parase == 1)
				return ERR_LINE;
			is_in_parase = 1;
			st.push(input[i]);
			break;
		case END_FIX:
			if(is_in_parase == 0)
				return ERR_LINE;
			is_in_parase = 0;
			st.pop_all(tmp_buf, MAX_STACK_LEN);
			err = do_parase(tmp_buf, tm);
			if(err)
				return err;
			strcat(out, tmp_buf);
			break;
		default:
			if(is_in_parase){
				st.push(input[i]);
			}else{
				out[strlen(out)] = input[i];
			}
		}
	}

	return 0;
}