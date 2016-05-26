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
 *	init		-		配置文件初始化
 *
 *	
 *	return
 *		==0			成功
 *		!=0			失败
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
			"第一条配置:监听端口[%d]转发到[%s:%d]",
			m_optdata[i].us_lisport,
			m_optdata[i].c_dstip,
			m_optdata[i].us_dstport);
	}

	return 0;
}

/**
 *	readoption		-		将配置文件内容加载到内存
 *
 *	
 *	return
 *		==0			成功
 *		!=0			失败
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
 *	getbyindex		-		在源串中查找某字符串第几次出现的位置
 *
 *	@src	[in]		源字符串
 *	@size	[in]		源串的长度
 *	@index	[in]		第几次出现
 *	@pfind	[in]		待查找字符串
 *	
 *	
 *	return
 *		!=NULL			找到的字符串
 *		==NULL			查找失败
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
