/**
 *	init/gcs_config.c
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
#include "typedef.h"
#include "native.h"
#include "gcs_page.h"
#include "gcs_errno.h"
#include "gcs_config.h"
#include "gcs_smif.h"

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: init/gcs_config.c,"
	"v 1.00 2013/10/28 15:14:40 yangling Exp $ (LBL)";
#endif

static struct gcs_config __gcs_config_object__;
#define	cfg_object()	(&__gcs_config_object__)

#define CONFIG_KEY_LENGTH	1024

#define	CONFIG_SECTION_NAME	"gcs_config"
#define	CONFIG_LISTEN_PORTS	"listen_ports"
#define	CONFIG_MAX_LINKS	"max_links"
#define	CONFIG_BPL			"bytes_per_link"
#define	CONFIG_CORE_THREADS	"core_threads"
#define	CONFIG_CONN_THREADS	"conn_threads"
#define	CONFIG_LOG_LEVEL	"log_level"
#define	CONFIG_RST_LEVEL	"rst_close"
#define	CONFIG_REUSEADDR	"reuseaddr"
#define	CONFIG_TIMEOUT		"timeout"

#define	CONFIG_SECTION_NAT	"nat"
#define	CONFIG_NAT_COUNT	"count"
#define	CONFIG_NAT_SRC		"in"
#define	CONFIG_NAT_DST		"out"

#define	NR_GCS_NAT			512
static struct gcs_nat *_gcs_nat_rules_;
static unsigned short nr_gcs_nat_rules;

/**	Follow the lines below for gcs config file
[gcs_config]
listen_ports	=80,443	;监听端口
max_links		=16384	;支持的最大连接数量
bytes_per_link	=65536	;每个链路的最大缓冲区大小(单位字节)
core_threads	=32		;核心工作线程数
conn_threads	=16		;连接工作线程数
reuseaddr		=0		;地址重绑定
timeout			=60		;超时时间检查
log_level		=0		;日志等级(隐藏参数,不告诉运营)
rst_close		=0		;强制关闭SOCKET(隐藏参数,不告诉运营)
*/

/**
 *	ref_config_object - 引用配置对象
 *
 *	return
 *		配置对象
 */
struct gcs_config*
ref_config_object()
{
	return cfg_object();
}

/**
 *	get_application_fname - 获取应用程序的全路径
 *
 *	@module:		模块句柄
 *	@abspath:		绝对路径
 *	@size:			绝对路径缓冲区大小
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
get_application_abspath(
	IN void *module, IN char *abspath, IN size_t size)
{
#ifdef WIN32
	
	if(!GetModuleFileName((HMODULE)module, abspath, size))
		return gen_errno(_OSerrno(), GCEOS);
	else
		return 0;
#else
#error "Not yet implementation"
#endif
}

/**
 *	format_config_abspath - 格式化配置文件名字
 *
 *	@abspath:		绝对路径
 *	@size:			绝对路径缓冲区大小
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
format_config_abspath(IN char *abspath, IN size_t size)
{
	int		i, len;
	int		rc;

	memset(abspath, 0, size);
	rc = get_application_abspath(NULL, abspath, size - 4);
	if(rc)
		return rc;

	len = strlen(abspath);

	for(i = len - 1; i >= 0; i--) {
		if(abspath[i] == '.') {
			abspath[i] = 0;
			break;
		}
	}

	if(i < 0)
		strcat(abspath, ".ini");
	else
		strcat(abspath + i, ".ini");

	return 0;
}

/**
 *	load_key_value - 提取配置项
 *
 *	@name:			配置文件名
 *	@section:		段名
 *	@key:			KEY名
 *	@def:			DEFAULT值
 *	@bret:			返回缓冲区
 *	@size:			返回缓冲区大小
 *
 *	return
 *		无
 */
void
load_key_value(IN char *name, IN char *section,
	IN char *key, IN char *def, OUT char *bret, OUT size_t size)
{
	char	bval[CONFIG_VALUE_LENGTH];
	wchar_t	wval[CONFIG_VALUE_LENGTH];
	int		rc;
	wchar_t	*sep = L";；\t\r\n ";
	wchar_t	*tkn;

	assert(size == CONFIG_VALUE_LENGTH);

	memset(bval, 0, sizeof(bval));
	memset(wval, 0, sizeof(wval));
	memset(bret, 0, size);

	rc = GetPrivateProfileString(
		section, key, def, bval, sizeof(bval), name);

	MultiByteToWideChar(CP_ACP, 0, bval, rc, wval, DINUM(wval));
	tkn = wcstok(wval, sep);
	if(IS_NOT_NULL(tkn))
		WideCharToMultiByte(CP_ACP, 0, tkn,
			wcslen(tkn), bret, size, NULL, NULL);
}

/**
 *	load_listen_ports - 提取端口配置
 *
 *	@name:			配置文件名
 *
 *	return
 *		无
 */
static void
load_listen_ports(IN char *name)
{
	char	value[CONFIG_VALUE_LENGTH];
	struct gcs_config	*obj = cfg_object();
	char				*tkn;
	const char			*sep = ",";

	load_key_value(name, 
		CONFIG_SECTION_NAME,
		CONFIG_LISTEN_PORTS, "80", value, sizeof(value));

	tkn = strtok(value, sep);
	while(tkn) {
		obj->lsnr_port[obj->nr_lsnrs++] =
				(unsigned short)strtoul(tkn, NULL, 0);
		tkn = strtok(NULL, sep);
	}
}

/**
 *	load_max_links - 提取最大连接数配置
 *
 *	@name:			配置文件名
 *
 *	return
 *		无
 */
static void
load_max_links(IN char *name)
{
	char	value[CONFIG_VALUE_LENGTH];
	struct gcs_config	*obj = cfg_object();

	load_key_value(name,
		CONFIG_SECTION_NAME,
		CONFIG_MAX_LINKS, "16384", value, sizeof(value));

	obj->nr_links = (unsigned short)strtoul(value, NULL, 0);

	/**	连接数,CLIENT与SERVER共用,所以要对配置做一下核查*/
	if(obj->nr_links >= 32768)
		obj->nr_links = 32768 - 1;

	if(obj->nr_links == 0)
		obj->nr_links = 16384;
}

/**
 *	load_core_threads - 提取核心线程数配置
 *
 *	@name:			配置文件名
 *
 *	return
 *		无
 */
static void
load_core_threads(IN char *name)
{
	char	value[CONFIG_VALUE_LENGTH];
	struct gcs_config	*obj = cfg_object();

	load_key_value(name,
		CONFIG_SECTION_NAME,
		CONFIG_CORE_THREADS, "32", value, sizeof(value));

	obj->core_ths = (unsigned short)strtoul(value, NULL, 0);
	if(obj->core_ths == 0)
		obj->core_ths = 32;
}

/**
 *	load_conn_threads - 提取连接线程数配置
 *
 *	@name:			配置文件名
 *
 *	return
 *		无
 */
static void
load_conn_threads(IN char *name)
{
	char	value[CONFIG_VALUE_LENGTH];
	struct gcs_config	*obj = cfg_object();

	load_key_value(name,
		CONFIG_SECTION_NAME,
		CONFIG_CONN_THREADS, "16", value, sizeof(value));

	obj->conn_ths = (unsigned short)strtoul(value, NULL, 0);
	if(obj->conn_ths == 0)
		obj->conn_ths = 8;
}

/**
 *	load_bytes_per_link - 提取每个链路的缓冲区大小配置
 *
 *	@name:			配置文件名
 *
 *	return
 *		无
 */
static void
load_bytes_per_link(IN char *name)
{
	char	value[CONFIG_VALUE_LENGTH];
	struct gcs_config	*obj = cfg_object();

	load_key_value(name,
		CONFIG_SECTION_NAME,	//"65536" 64KB
		CONFIG_BPL, "65536", value, sizeof(value));

	obj->bytes_per_link = strtoul(value, NULL, 0);

	if(obj->bytes_per_link == 0)
		obj->bytes_per_link = MPAGE_SIZE;
	else {
		if(obj->bytes_per_link % MPAGE_SIZE)
			obj->bytes_per_link +=
			(MPAGE_SIZE - obj->bytes_per_link % MPAGE_SIZE);
	}
}

/**
 *	load_log_level - 提取日志等级配置
 *
 *	@name:			配置文件名
 *
 *	return
 *		无
 */
static void
load_log_level(IN char *name)
{
	char	value[CONFIG_VALUE_LENGTH];
	struct gcs_config	*obj = cfg_object();

	load_key_value(name,
		CONFIG_SECTION_NAME,
		CONFIG_LOG_LEVEL, "0", value, sizeof(value));

	obj->log_level = (unsigned short)strtoul(value, NULL, 0);

	/**
	 *	禁止超过支持人员的日志登记配置,
	 *	因为开发级别的日志会非常的多
	 */
	if(obj->log_level > LOG_LEVEL_SUPPORT)
		obj->log_level = 0;
}

/**
 *	load_rst_close - 提取连接关闭模式配置
 *
 *	@name:			配置文件名
 *
 *	return
 *		无
 */
static void
load_rst_close(IN char *name)
{
	char	value[CONFIG_VALUE_LENGTH];
	struct gcs_config	*obj = cfg_object();

	load_key_value(name,
		CONFIG_SECTION_NAME,
		CONFIG_RST_LEVEL, "0", value, sizeof(value));

	obj->rst_close = (unsigned short)strtoul(value, NULL, 0);
}

/**
 *	load_reuseaddr - 提取地址重绑定配置
 *
 *	@name:			配置文件名
 *
 *	return
 *		无
 */
static void
load_reuseaddr(IN char *name)
{
	char	value[CONFIG_VALUE_LENGTH];
	struct gcs_config	*obj = cfg_object();

	load_key_value(name,
		CONFIG_SECTION_NAME,
		CONFIG_REUSEADDR, "0", value, sizeof(value));

	obj->reuseaddr = (unsigned short)strtoul(value, NULL, 0);

	if(obj->reuseaddr) {
		write_log(GCS_MODULE_ID,
			LOG_TYPE_WARN, LOG_LEVEL_GENERAL,
			"warning:  you allows the socket to be bound "
			"to an address that is already in use.  you "
			"can ignore this warning if you know the behavior. "
			"otherwise i suggest you should keep "
			"the reuseaddr option is closed");
	}
}

/**
 *	load_timeout - 提取超时时间配置
 *
 *	@name:			配置文件名
 *
 *	return
 *		无
 */
static void
load_timeout(IN char *name)
{
	char	value[CONFIG_VALUE_LENGTH];
	struct gcs_config	*obj = cfg_object();

	load_key_value(name,
		CONFIG_SECTION_NAME,
		CONFIG_TIMEOUT, "60", value, sizeof(value));

	obj->timeout = (unsigned short)strtoul(value, NULL, 0);
}

/**
 *	nat_compare - NAT比较函数
 *
 *	@elem1:		参数一
 *	@elem2:		参数一
 *
 *	return
 *		>	0	参数一大于参数二
 *		==	0	参数一等于参数二
 *		<	0	参数一小于参数二
 */
static int
nat_compare(IN const void *elem1, IN const void *elem2)
{
	int	rc;
	struct gcs_nat *v1 = (struct gcs_nat *)elem1;
	struct gcs_nat *v2 = (struct gcs_nat *)elem2;

	rc = strcmp(v1->src_ptr, v2->src_ptr);
	if(!rc)
		return (int)((int)v1->src_port - (int)v2->src_port);
	else
		return rc;
}

/**
 *	load_nat - 提取NAT的配置
 *
 *	@name:			配置文件名
 *
 *	return
 *		0			成功
 *		!0			失败
 */
static int
load_nat(IN char *name)
{
	char	value[CONFIG_VALUE_LENGTH];
	char	value_src[CONFIG_VALUE_LENGTH];
	char	value_dst[CONFIG_VALUE_LENGTH];
	char	key_src[CONFIG_KEY_LENGTH];
	char	key_dst[CONFIG_KEY_LENGTH];
	const char seps[] = "\r\n\t :";
	char	*tkn;
	int		i;
	unsigned short	nr_rules;
	struct gcs_nat	nat;

	_gcs_nat_rules_	= NULL;
	nr_gcs_nat_rules = 0;

	load_key_value(name,
		CONFIG_SECTION_NAT,
		CONFIG_NAT_COUNT, "0", value, sizeof(value));

	nr_rules = (unsigned short)strtoul(value, NULL, 0);

	if(nr_rules > NR_GCS_NAT) {
		write_log(GCS_MODULE_ID,
			LOG_TYPE_WARN, LOG_LEVEL_GENERAL,
			"nat rules too many. "
			"only accepted %d rules. "
			"above %d rules will be truncate",
			NR_GCS_NAT, NR_GCS_NAT);
		nr_rules = NR_GCS_NAT;
	}

	if(nr_rules == 0)
		return 0;

	_gcs_nat_rules_ = zmalloc(nr_rules * sizeof(struct gcs_nat));
	if(IS_NULL(_gcs_nat_rules_)) {
		write_log(GCS_MODULE_ID,
			LOG_TYPE_ERROR, LOG_LEVEL_GENERAL,
			"An error occurred while allocating "
			"memory for nat rules");
		return gen_errno(0, GCENOMEM);
	}

	for(i = 0; i < nr_rules; i++) {
		sprintf(key_src, "%s%d", CONFIG_NAT_SRC, i);
		sprintf(key_dst, "%s%d", CONFIG_NAT_DST, i);

		load_key_value(name,
			CONFIG_SECTION_NAT,
			key_src, "", value_src, sizeof(value_src));

		load_key_value(name,
			CONFIG_SECTION_NAT,
			key_dst, "", value_dst, sizeof(value_dst));

		if(strlen(value_src) == 0) {
			write_log(GCS_MODULE_ID,
				LOG_TYPE_WARN, LOG_LEVEL_GENERAL,
				"nat rule %d missing source address "
				"info, discarded", i);
			continue;
		}

		if(strlen(value_dst) == 0) {
			write_log(GCS_MODULE_ID,
				LOG_TYPE_WARN, LOG_LEVEL_GENERAL,
				"nat rule %d missing destination address "
				"info, discarded", i);
			continue;
		}

		memset(&nat, 0, sizeof(nat));

		/*	源IP*/
		tkn = strtok(value_src, seps);
		if(IS_NULL(tkn)) {
			write_log(GCS_MODULE_ID,
				LOG_TYPE_WARN, LOG_LEVEL_GENERAL,
				"nat rule %d missing source ip "
				"info, discarded", i);
			continue;
		} else
			strncpy(nat.src_addr, tkn,
				min(MAX_DOMAIN_LENGTH, strlen(tkn)));

		/*	源PORT*/
		tkn = strtok(NULL, seps);
		if(IS_NULL(tkn)) {
			write_log(GCS_MODULE_ID,
				LOG_TYPE_WARN, LOG_LEVEL_GENERAL,
				"nat rule %d missing source port "
				"info, discarded", i);
			continue;
		} else
			nat.src_port = (unsigned short)strtoul(tkn, NULL, 0);

		nat.src_port = htons(nat.src_port);

		/*	目标IP*/
		tkn = strtok(value_dst, seps);
		if(IS_NULL(tkn)) {
			write_log(GCS_MODULE_ID,
				LOG_TYPE_WARN, LOG_LEVEL_GENERAL,
				"nat rule %d missing destination ip "
				"info, discarded", i);
			continue;
		} else
			strncpy(nat.dst_addr, tkn,
				min(MAX_DOMAIN_LENGTH, strlen(tkn)));

		/*	目标PORT*/
		tkn = strtok(NULL, seps);
		if(IS_NULL(tkn)) {
			write_log(GCS_MODULE_ID,
				LOG_TYPE_WARN, LOG_LEVEL_GENERAL,
				"nat rule %d missing destination port "
				"info, discarded", i);
			continue;
		} else
			nat.dst_port = (unsigned short)strtoul(tkn, NULL, 0);

		nat.dst_port = htons(nat.dst_port);

		memcpy(_gcs_nat_rules_ + nr_gcs_nat_rules, &nat, sizeof(nat));
		_gcs_nat_rules_[nr_gcs_nat_rules].src_ptr = 
				_gcs_nat_rules_[nr_gcs_nat_rules].src_addr;
		_gcs_nat_rules_[nr_gcs_nat_rules].dst_ptr =
				_gcs_nat_rules_[nr_gcs_nat_rules].dst_addr;
		nr_gcs_nat_rules++;
	}

	qsort(_gcs_nat_rules_, nr_gcs_nat_rules,
			sizeof(struct gcs_nat), nat_compare);

	return 0;
}

/**
 *	lookup_nat_rules - 查找NAT规则
 *
 *	@src_ipv4:		源IP地址
 *	@src_port:		源端口(big endian)
 *
 *	return
 *		NULL		没找到
 *		!NULL		找到了
 */
struct gcs_nat *
lookup_nat_rules(
	IN char *src_ipv4, IN unsigned short src_port)
{
	struct gcs_nat	key;

	if(IS_NULL(_gcs_nat_rules_) ||
		nr_gcs_nat_rules == 0)
		return NULL;

	key.src_ptr = src_ipv4;
	key.src_port= src_port;


	return bsearch(&key,
				_gcs_nat_rules_, nr_gcs_nat_rules, 
				sizeof(struct gcs_nat), nat_compare);
}

/**
 *	trace_config_log - 打印配置日志
 *
 *	return
 *		无
 */
static void
trace_config_log()
{
	int					i;
	struct gcs_config	*obj = cfg_object();

	write_log(GCS_MODULE_ID,
		LOG_TYPE_INFO, LOG_LEVEL_GENERAL,
		"core threads: %d", obj->core_ths);
	
	write_log(GCS_MODULE_ID,
		LOG_TYPE_INFO, LOG_LEVEL_GENERAL,
		"conn threads: %d", obj->conn_ths);
	
	write_log(GCS_MODULE_ID,
		LOG_TYPE_INFO, LOG_LEVEL_GENERAL,
		"max links: %d", obj->nr_links);
	
	write_log(GCS_MODULE_ID,
		LOG_TYPE_INFO, LOG_LEVEL_GENERAL,
		"bytes per link: %d", obj->bytes_per_link);

	write_log(GCS_MODULE_ID,
		LOG_TYPE_INFO, LOG_LEVEL_GENERAL,
		"log level: %d", obj->log_level);

	write_log(GCS_MODULE_ID,
		LOG_TYPE_INFO, LOG_LEVEL_GENERAL,
		"rst close: %d", obj->rst_close);

	write_log(GCS_MODULE_ID,
		LOG_TYPE_INFO, LOG_LEVEL_GENERAL,
		"reuseaddr: %d", obj->reuseaddr);

	write_log(GCS_MODULE_ID,
		LOG_TYPE_INFO, LOG_LEVEL_GENERAL,
		"timeout: %d", obj->timeout);

	for(i = 0; i < obj->nr_lsnrs; i++)
		write_log(GCS_MODULE_ID,
			LOG_TYPE_INFO, LOG_LEVEL_GENERAL,
			"listen port: %d", obj->lsnr_port[i]);
}

/**
 *	global_config_init - 全局配置初始化
 *
 *	return
 *		无
 */
int
global_config_init()
{
	char				name[MAX_PATH];
	int					rc;
	struct gcs_config	*obj = cfg_object();

	memset(obj, 0, sizeof(struct gcs_config));

	rc = format_config_abspath(name, sizeof(name));
	if(rc)
		return rc;

	load_listen_ports(name);

	load_max_links(name);

	load_core_threads(name);

	load_conn_threads(name);

	load_bytes_per_link(name);

	load_log_level(name);

	load_rst_close(name);

	load_reuseaddr(name);

	load_timeout(name);

	rc = load_nat(name);
	if(rc)
		return rc;

	trace_config_log();

	return 0;
}