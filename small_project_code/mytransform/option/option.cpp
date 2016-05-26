#include <stdio.h>
#include <assert.h>
#include <windows.h>
#include "option.h"
#include "error.h"
#include "global.h"

MOption::MOption()
{
	optioncount = 0;
}

MOption::~MOption()
{

}


/**
 *	init		-		�����ļ���ʼ��
 *
 *	
 *	return
 *		==0			�ɹ�
 *		!=0			ʧ��
 */
int	MOption::init(int argc, char *argv[])
{
	int			i;
	int			begin = 1;
	
	if((argc-1)%3 != 0)
		return BUILD_ERROR(0, E_PARAM);

	optioncount = (argc-1)/3;
	for( i = 0; i < optioncount; ++i){
		m_optdata[i].us_lisport = atoi(argv[begin++]);
		strncpy(m_optdata[i].c_dstip, argv[begin++], MAX_IP_LEN-1);
		m_optdata[i].us_dstport = atoi(argv[begin++]);
	}

	for( i = 0; i < optioncount; ++i){
		global_log.writeLog(LOG_TYPE_INFO, 
			"��һ������:�����˿�[%d]ת����[%s:%d]",
			m_optdata[i].us_lisport,
			m_optdata[i].c_dstip,
			m_optdata[i].us_dstport);
	}

	return 0;
}

/**
 *	readoption		-		�������ļ����ݼ��ص��ڴ�
 *
 *	
 *	return
 *		==0			�ɹ�
 *		!=0			ʧ��
 */
// int	MOption::readoption(void)
// {
// 	assert(m_pfile != NULL);
// 
// 	char			buf[1024] = {0};
// 	int				i = 0;
// 	int				rc;
// 	char			*ptr;
// 
// 	while((fgets(buf, sizeof(buf)-1, m_pfile) != NULL) && i < MAX_OPTION){
// 		if((ptr = getbyindex(buf, sizeof(buf), 1, ":")) == NULL)
// 			return BUILD_ERROR(0, E_OPTION);
// 		m_optdata[i].us_lisport = atoi(ptr+1);
// 
// 		if((ptr = getbyindex(buf, sizeof(buf), 2, ":")) == NULL)
// 			return BUILD_ERROR(0, E_OPTION);
// 		m_optdata[i].us_dstport = atoi(ptr+1);
// 
// 		if((ptr = getbyindex(buf, sizeof(buf), 3, ":")) == NULL)
// 			return BUILD_ERROR(0, E_OPTION);
// 		strncpy(m_optdata[i].c_dstip, ptr+1, MAX_IP_LEN - 1);
// 
// 		++optioncount;
// 
// 		memset(buf, 0, sizeof(buf));
// 	}
// 
// 	return 0;
// }

/**
 *	getbyindex		-		��Դ���в���ĳ�ַ����ڼ��γ��ֵ�λ��
 *
 *	@src	[in]		Դ�ַ���
 *	@size	[in]		Դ���ĳ���
 *	@index	[in]		�ڼ��γ���
 *	@pfind	[in]		�������ַ���
 *	
 *	
 *	return
 *		!=NULL			�ҵ����ַ���
 *		==NULL			����ʧ��
 */
// char *MOption::getbyindex(char *src, size_t size, unsigned short index, char* pfind)
// {
// 
// 	char			*ptr;
// 
// 	if(src == NULL || pfind == NULL)
// 		return NULL;
// 
// 	ptr = src;
// 	for(int i = 0; i < size; ++i){
// 		ptr = strstr(ptr, pfind);
// 
// 		if(ptr == NULL)
// 			return NULL;
// 	}
// 
// 	if(i == size+1)
// 		return NULL;
// }
