#ifndef	_YYYY2CHAR_H_
#define _YYYY2CHAR_H_
#include <time.h>

//	����ֵ��== 0 �ɹ� != 0 ʧ��
int	yyyy2char(const char	*input,			/*	��ת�����ַ����磺csi-<YYYY>-<MM>-<DD>.txt	*/
			  time_t		tm,				/*	ʱ��										*/
			  char			*out,			/*	ת������洢λ��							*/
			  size_t		out_size		/*	�洢�������								*/
			  );

#endif