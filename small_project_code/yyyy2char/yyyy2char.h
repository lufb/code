#ifndef	_YYYY2CHAR_H_
#define _YYYY2CHAR_H_
#include <time.h>

//	返回值：== 0 成功 != 0 失败
int	yyyy2char(const char	*input,			/*	待转换的字符串如：csi-<YYYY>-<MM>-<DD>.txt	*/
			  time_t		tm,				/*	时间										*/
			  char			*out,			/*	转换结果存储位置							*/
			  size_t		out_size		/*	存储结果长度								*/
			  );

#endif