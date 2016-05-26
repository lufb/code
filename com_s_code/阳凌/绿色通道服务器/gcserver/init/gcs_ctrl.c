/**
 *	init/gcs_ctrl.c
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#include "features.h"
#include <stdio.h>
#include "typedef.h"
#include "native.h"
#include "gcs_ctrl.h"
#include "gcs_version.h"
#include "gcs_smif.h"
#include "gcs_main.h"
#include "gcs_conn.h"
#include "gcs_lsnr.h"
#include "gcs_link.h"
#include "gcs_page.h"
#include "gcs_config.h"

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: init/gcs_ctrl.c,"
	"v 1.00 2013/11/01 16:03:40 yangling Exp $ (LBL)";
#endif

/**
 *	do_version_cmd - do 版本命令
 *
 *	@cmd_no:		命令序列号
 *	@argc:			命令个数
 *	@argv:			命令参数
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
do_version_cmd(IN unsigned int cmd_no, IN int argc, IN char *argv[])
{
	char	verbuf[256];
	char	cmdbuf[_4X_PKG_DATA_SIZE_];

	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	_snprintf(cmdbuf, sizeof(cmdbuf), "%s %s", GCSERVER_NAME, 
			global_public_version_str(verbuf, sizeof(verbuf)));

	smif_ctrl_cmd_println(cmd_no, cmdbuf);

	return 0;
}

/**
 *	do_xver_cmd - do xver命令
 *
 *	@cmd_no:		命令序列号
 *	@argc:			命令个数
 *	@argv:			命令参数
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
do_xver_cmd(IN unsigned int cmd_no, IN int argc, IN char *argv[])
{
	char	verbuf[256];
	char	cmdbuf[_4X_PKG_DATA_SIZE_];

	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	_snprintf(cmdbuf, sizeof(cmdbuf), "%s %s", GCSERVER_NAME, 
			global_private_version_str(verbuf, sizeof(verbuf)));

	smif_ctrl_cmd_println(cmd_no, cmdbuf);

	return 0;
}

/**
 *	do_conn_cmd - do conn命令
 *
 *	@cmd_no:		命令序列号
 *	@argc:			命令个数
 *	@argv:			命令参数
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
do_conn_cmd(IN unsigned int cmd_no, IN int argc, IN char *argv[])
{
	char	cmdbuf[_4X_PKG_DATA_SIZE_];

	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	make_conn_module_ctrl_buffer(cmdbuf, sizeof(cmdbuf));

	smif_ctrl_cmd_println(cmd_no, cmdbuf);

	return 0;
}

/**
 *	do_lsnr_cmd - do lsnr命令
 *
 *	@cmd_no:		命令序列号
 *	@argc:			命令个数
 *	@argv:			命令参数
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
do_lsnr_cmd(IN unsigned int cmd_no, IN int argc, IN char *argv[])
{
	char	cmdbuf[_4X_PKG_DATA_SIZE_];

	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	make_lsnr_module_ctrl_buffer(cmdbuf, sizeof(cmdbuf));

	smif_ctrl_cmd_println(cmd_no, cmdbuf);

	return 0;
}

/**
 *	do_link_cmd - do lsnr命令
 *
 *	@cmd_no:		命令序列号
 *	@argc:			命令个数
 *	@argv:			命令参数
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
do_link_cmd(IN unsigned int cmd_no, IN int argc, IN char *argv[])
{
	char	cmdbuf[_4X_PKG_DATA_SIZE_];

	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	make_link_module_ctrl_buffer(cmdbuf, sizeof(cmdbuf));

	smif_ctrl_cmd_println(cmd_no, cmdbuf);

	return 0;
}

/**
 *	do_ts_cmd - do ts命令
 *
 *	@cmd_no:		命令序列号
 *	@argc:			命令个数
 *	@argv:			命令参数
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
do_ts_cmd(IN unsigned int cmd_no, IN int argc, IN char *argv[])
{
	char	cmdbuf[_4X_PKG_DATA_SIZE_];

	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	make_ts_ctrl_buffer(cmdbuf, sizeof(cmdbuf));

	smif_ctrl_cmd_println(cmd_no, cmdbuf);

	return 0;
}

/**
 *	do_dts_cmd - do dts命令
 *
 *	@cmd_no:		命令序列号
 *	@argc:			命令个数
 *	@argv:			命令参数
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
do_dts_cmd(IN unsigned int cmd_no, IN int argc, IN char *argv[])
{
	char	cmdbuf[_4X_PKG_DATA_SIZE_];

	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	make_dts_ctrl_buffer(cmdbuf, sizeof(cmdbuf));

	smif_ctrl_cmd_println(cmd_no, cmdbuf);

	return 0;
}

/**
 *	do_page_cmd - do page命令
 *
 *	@cmd_no:		命令序列号
 *	@argc:			命令个数
 *	@argv:			命令参数
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
do_page_cmd(IN unsigned int cmd_no, IN int argc, IN char *argv[])
{
	char	cmdbuf[_4X_PKG_DATA_SIZE_];

	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	make_page_ctrl_buffer(cmdbuf, sizeof(cmdbuf));

	smif_ctrl_cmd_println(cmd_no, cmdbuf);

	return 0;
}

/**
 *	do_timeout_cmd - do timeout命令
 *
 *	@cmd_no:		命令序列号
 *	@argc:			命令个数
 *	@argv:			命令参数
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
do_timeout_cmd(IN unsigned int cmd_no, IN int argc, IN char *argv[])
{
	unsigned short	timeout;

	if(argc != 2)
		return -1;

	if(argv[1] == NULL)
		return -2;

	timeout = (unsigned short)strtoul(argv[1], NULL, 0);

	ref_config_object()->timeout = timeout;

	smif_ctrl_cmd_println(cmd_no,
		"%s command successfully applied "
		"new parameter is %d", argv[0], timeout);

	return 0;
}

/**
 *	do_log_level_cmd - do loglevel命令
 *
 *	@cmd_no:		命令序列号
 *	@argc:			命令个数
 *	@argv:			命令参数
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
do_log_level_cmd(IN unsigned int cmd_no, IN int argc, IN char *argv[])
{
	unsigned short	log_level;

	if(argc != 2)
		return -1;

	if(argv[1] == NULL)
		return -2;

	log_level = (unsigned short)strtoul(argv[1], NULL, 0);
	if(log_level > LOG_LEVEL_SUPPORT)
		log_level = LOG_LEVEL_SUPPORT;

	ref_config_object()->log_level = log_level;

	smif_ctrl_cmd_println(cmd_no,
		"%s command successfully applied "
		"new parameter is %d", argv[0], log_level);

	return 0;
}

/**
 *	do_rst_close_cmd - do rst close命令
 *
 *	@cmd_no:		命令序列号
 *	@argc:			命令个数
 *	@argv:			命令参数
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
do_rst_close_cmd(IN unsigned int cmd_no, IN int argc, IN char *argv[])
{
	unsigned short	rst_close;

	if(argc != 2)
		return -1;

	if(argv[1] == NULL)
		return -2;

	rst_close = (unsigned short)strtoul(argv[1], NULL, 0);

	ref_config_object()->rst_close = rst_close;

	smif_ctrl_cmd_println(cmd_no,
		"%s command successfully applied "
		"new parameter is %d", argv[0], rst_close);

	return 0;
}

/**
 *	do_config_cmd - do config命令
 *
 *	@cmd_no:		命令序列号
 *	@argc:			命令个数
 *	@argv:			命令参数
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
do_config_cmd(IN unsigned int cmd_no, IN int argc, IN char *argv[])
{
	char	cmdbuf[_4X_PKG_DATA_SIZE_];
	int		i, rc;
	struct gcs_config	*obj = ref_config_object();
	
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	rc = _snprintf(cmdbuf, sizeof(cmdbuf),
				"core threads          = %u\n"
				"conn threads          = %u\n"
				"max links             = %u\n"
				"bytes per link        = %u\n"
				"log level             = %u\n"
				"rst close             = %u\n"
				"reuseaddr             = %u\n"
				"timeout               = %u\n",
				obj->core_ths,
				obj->conn_ths,
				obj->nr_links,
				obj->bytes_per_link,
				obj->log_level,
				obj->rst_close,
				obj->reuseaddr,
				obj->timeout);

	for(i = 0; i < min(NR_LSNR_PORT, obj->nr_lsnrs); i++)
	{
		rc += _snprintf(cmdbuf + rc, sizeof(cmdbuf) - rc,
				"listen port           = %u\n", obj->lsnr_port[i]);
	}
	

	smif_ctrl_cmd_println(cmd_no, cmdbuf);

	return 0;
}

static int
do_help_cmd(IN unsigned int cmd_no, IN int argc, IN char *argv[]);

static struct ctrl_cmd {
	int		forbidden;
	char	*cmd;
	int		(*ctrl_cmd_fn)(
				IN unsigned int cmd_no,
				IN int argc, IN char *argv[]);
	char	*tip;
}ctrl_command_tab[] = 
{
	{0, "version",		do_version_cmd, "show system version"},
	{1, "xver",			do_xver_cmd, ""},
	{0, "conn",			do_conn_cmd, "show statistics for connect to server"},
	{0, "lsnr",			do_lsnr_cmd, "show statistics for accept from client"},
	{0, "link",			do_link_cmd, "show statistics for transmit data"},
	{0, "ts",			do_ts_cmd, "show thread base status"},
	{0, "dts",			do_dts_cmd, "show thread detail status"},
	{0, "page",			do_page_cmd, "show statistics for memory page"},
	{0, "help",			do_help_cmd, "help"},
	{0, "gcstimeout",	do_timeout_cmd, "change timeout parameter"},
	{0,	"loglevel",		do_log_level_cmd, "change log level parameter"},
	{0, "config",		do_config_cmd, "show gcserver all configuration"},
	{0, "rstclose",		do_rst_close_cmd, "change rst close parameter"},
};

/**
 *	do_help_cmd - do help命令
 *
 *	@cmd_no:		命令序列号
 *	@argc:			命令个数
 *	@argv:			命令参数
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
do_help_cmd(IN unsigned int cmd_no, IN int argc, IN char *argv[])
{
	int		rc = 0, i;
	char	cmdbuf[_4X_PKG_DATA_SIZE_];
	
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	for(i = 0; i < DINUM(ctrl_command_tab); i++)
	{
		if(ctrl_command_tab[i].forbidden)
			continue;

		rc += _snprintf(cmdbuf + rc, sizeof(cmdbuf) - rc,
					"%-10s %s\n",
					ctrl_command_tab[i].cmd,
					ctrl_command_tab[i].tip);
	}
	
	smif_ctrl_cmd_println(cmd_no, cmdbuf);
	
	return 0;
}


static int
ctrl_command_compare(IN const void *elem1, IN const void *elem2)
{
	struct ctrl_cmd *v1 = (struct ctrl_cmd *)elem1;
	struct ctrl_cmd *v2 = (struct ctrl_cmd *)elem2;

	return strcmp(v1->cmd, v2->cmd);
}

/**
 *	global_ctrl_command_entry - 控制台命令入口
 *
 *	@cmd_no:		命令序列号
 *	@argc:			命令个数
 *	@argv:			命令参数
 *
 *	return
 *		无
 */
void
global_ctrl_command_entry(
	IN unsigned int cmd_no, IN int argc, IN char *argv[])
{
	char			cmd[256];
	int				rc;
	struct ctrl_cmd key;
	struct ctrl_cmd	*cc;

	if(argc < 1) {
		smif_ctrl_cmd_println(cmd_no, "typing parameters error %d", argc);
		return;
	}

	memset(cmd, 0, sizeof(cmd));
	strncpy(cmd, argv[0], sizeof(cmd) - 1);

	key.cmd = cmd;

	cc = bsearch(&key,
			ctrl_command_tab,
			DINUM(ctrl_command_tab),
			sizeof(struct ctrl_cmd),
			ctrl_command_compare);
	if(IS_NOT_NULL(cc) &&
		IS_NOT_NULL(cc->cmd))
		rc = cc->ctrl_cmd_fn(cmd_no, argc, argv);
	else {
		smif_ctrl_cmd_println(cmd_no, "%s: command not found", argv[0]);
		return;
	}

	if(rc) {
		smif_ctrl_cmd_println(
			cmd_no, "An error occurred "
			"while executing command '%s' E->0x%08X", argv[0], rc);
	}
}

/**
 *	global_ctrl_command_init - 全局初始化控制命令
 *
 *	return
 *		无
 */
void
global_ctrl_command_init()
{
	qsort(ctrl_command_tab,
			DINUM(ctrl_command_tab),
			sizeof(struct ctrl_cmd),
			ctrl_command_compare);
}