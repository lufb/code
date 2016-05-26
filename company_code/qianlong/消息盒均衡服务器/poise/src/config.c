/**
 *	src/config.c
 *
 *	Copyright (C) 2012 阳凌 <yl.tienon@gmail.com>
 *
 *	定义关于用户配置相关的函数方法的实现
 *
 *	修改历史:
 *
 *	2013-01-18 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmail.com>
 */

#include	"config.h"
#include	"request.h"
#include	"local.h"

static HANDLE poise_module_handle;

static struct _ulimit ulimit;

#define	DEFAULT_PROXY_LIMIT				30000
#define	DEFAULT_UPGRADE_LIMIT			8192

BOOL SPSTDCALL DllMain(HANDLE module, DWORD reason4call, LPVOID reserved)
{
	poise_module_handle = module;

	return TRUE;
}

/**
 *	get_ulimit - 得到限制信息
 *
 *	return
 *		限制信息数据结构
 */
struct _ulimit *get_ulimit()
{
	return &ulimit;
}

/**
 *	config_init - 配置初始化
 *
 *	return
 *		无
 */
void config_init()
{
	struct _util_ops *	util = get_util_ops(mbi_sp_interface);
	int					rc, i;
	void			*	preg;
	char				ini[MAX_PATH];
	char				temp[8192];

	memset(ini, 0, sizeof(ini));
	memset(&ulimit, 0, sizeof(ulimit));

	rc = util->get_module_fname(poise_module_handle, ini, sizeof(ini));
	if(IS_FAILURE(rc))
		return;

	for(i = strlen(ini) -1; i > 0 ; --i) {
		if(ini[i] == '.' ) {
			ini[i] = 0;
			break;
		}
	}
	strcat(ini, ".ini");

	preg = util->_cfgcomp(ini);
	if(IS_NOT_NULL(preg)) {
		memset(temp, 0, sizeof(temp));
		util->_cfgexec(preg, "ulimit", "proxy_limit", temp, sizeof(temp) - 1);
		ulimit.proxy_limit = (unsigned short)strtoul(temp, NULL, 0);
		if(ulimit.proxy_limit == 0)
			ulimit.proxy_limit = DEFAULT_PROXY_LIMIT;

		memset(temp, 0, sizeof(temp));
		util->_cfgexec(preg, "ulimit", "upgrade_limit", temp, sizeof(temp) - 1);
		ulimit.upgrade_limit = (unsigned short)strtoul(temp, NULL, 0);
		if(ulimit.upgrade_limit == 0)
			ulimit.upgrade_limit = DEFAULT_UPGRADE_LIMIT;

		util->_cfgfree(preg);
	} else {
		ulimit.proxy_limit		= DEFAULT_PROXY_LIMIT;
		ulimit.upgrade_limit	= DEFAULT_UPGRADE_LIMIT;
	}
}