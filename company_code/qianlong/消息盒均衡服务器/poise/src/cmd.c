#include "cmd.h"
#include <stdio.h>


static int do_help_cmd(
						  IN unsigned int cmd_no, IN char *argv[], IN int argc)
{
	int					i, rc = 0;
	struct _util_ops	*util = &(mbi_sp_interface->util_ops);
	char				*cmds[] =
	{
		"help", "version", "servinfo"
	};
	
	char				*tips[] =
	{
		"提供 POISE 命令的帮助信息",
		"提供 POISE 的版本信息",
		"提供 POISE 中代理与升级服务器的详细信息"
	};
	char				cmdbuf[MAX_PKG_BODY_SIZE];
	
	if(sizeof(tips) != sizeof(cmds)) {
		util->ctl_cmd_println(cmd_no, "POISE Panic");
		return 0;
	}
	
	
	for(i = 0; i < sizeof(cmds) / sizeof(char*); i++) {
		rc += _snprintf(cmdbuf + rc, sizeof(cmdbuf) - rc,
			"%-12s    %s\n", cmds[i], tips[i]);
	}
	
	util->ctl_cmd_println(cmd_no, cmdbuf);
	
	return 0;
}

static int do_version_cmd(
						  IN unsigned int cmd_no, IN char *argv[], IN int argc)
{
	struct _util_ops	*util = &(mbi_sp_interface->util_ops);
	unsigned int		version;
	char				cmdbuf[MAX_PKG_BODY_SIZE];
	
	version = poise_version();
	
	_snprintf(cmdbuf, sizeof(cmdbuf),
#ifdef _DEBUG
		"%sD[V%3.2f B%03d]",
#else
		"%sR[V%3.2f B%03d]",
#endif
		POISE_NAME, (float)(version>>16) / 100, version & 0xFFFF );
	
	util->ctl_cmd_println(cmd_no, cmdbuf);
	
	return 0;
}

static int printDataLn(
						   IN unsigned int cmd_no, IN char *argv[], IN int argc)
{
	int					totalServ = 0;//用来计算总的有几个服务器
	int					pos = 0;
	struct _util_ops	*util = &(mbi_sp_interface->util_ops);
	struct _thread_ops	*ths  = get_thread_ops(mbi_sp_interface);
	struct list_head	*tmp1;
	struct list_head	*tmp2;
	AGENT_LIST			*pFirstList;
	UPDATE_LIST			*p;
	SRV_INFO			*pSecondList;
	char				cmdbuf[7000];//防止数据万一越界，少定义点大小

	_snprintf(cmdbuf, sizeof(cmdbuf),
		"+++++++++++++++++++++代理服务器++++++++++++++++++++++\n");
	ths->nsleep(10); util->ctl_cmd_print(cmd_no, cmdbuf);
	
	list_for_each(tmp1, &agentListHead)
	{
		pFirstList = list_entry(tmp1, AGENT_LIST, roomList);
		_snprintf(cmdbuf, sizeof(cmdbuf),  "-roomNo:%d-totalBalance:%d-totablMachine:%d-\n",pFirstList->usRoomNo,pFirstList->ulTotalBalance,pFirstList->usTotalMachine);
		ths->nsleep(10); util->ctl_cmd_print(cmd_no, cmdbuf);

		list_for_each(tmp2, &(pFirstList->machineListHead))
		{
			++totalServ;
			pos = 0;
			pSecondList = list_entry(tmp2, SRV_INFO, list);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "ROOMNO:%d\n", pSecondList->m_po.m_tStaticInfo.m_siMRoomNo);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "SRVNO:%d\n", pSecondList->m_po.m_tStaticInfo.m_siPNo);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "MAXLOADCAP:%lu\n", pSecondList->m_po.m_tStaticInfo.m_uiMaxLoadCap);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "IP:%16.16s\n", pSecondList->m_po.m_tStaticInfo.m_szServiceIP);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "PORT:%u\n", pSecondList->m_po.m_tStaticInfo.m_usServicePort);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "CPUUSED:%u\n", pSecondList->m_po.m_tStateInfo.m_ucCPUPer);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "MEMUSED:%u\n", pSecondList->m_po.m_tStateInfo.m_ucMemPer);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "CURLINK:%u\n", pSecondList->m_po.m_tStateInfo.m_uiCurrLink);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "BALANCEVALUE:%u\n", pSecondList->m_usBalanceValue);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "TYPE:%c\n", pSecondList->m_cType);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "LINKNO:%u\n", pSecondList->m_uiLinkNo);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "TESTSPEED:%u\n",pSecondList->m_po.m_tStaticInfo.m_usTestSpeedPort);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "AVAILABLE:%d\n\n", pSecondList->m_cIsAvaliable);
			ths->nsleep(10); util->ctl_cmd_print(cmd_no, cmdbuf);
		}
	}

	_snprintf(cmdbuf, sizeof(cmdbuf), 
		"+++++++++++++++++++++升级服务器++++++++++++++++++++++\n");
	ths->nsleep(10); util->ctl_cmd_print(cmd_no, cmdbuf);

	list_for_each(tmp1, &updateListHead)
	{
		p = list_entry(tmp1, UPDATE_LIST, roomList);
		_snprintf(cmdbuf, sizeof(cmdbuf)-pos, "-roomNo:%d-totalBalance:%d-totalMachine:%d-\n",p->usRoomNo, p->ulTotalBalance,p->usTotalMachine);
		ths->nsleep(10); util->ctl_cmd_print(cmd_no, cmdbuf);
		list_for_each(tmp2, &(p->machineListHead))
		{
			++totalServ;
			pos = 0;
			pSecondList = list_entry(tmp2, SRV_INFO, list);
			
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "ROOMNO:%d\n", pSecondList->m_up.m_tStaticInfo.m_siMRoomNo);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "SRVNO:%d\n", pSecondList->m_up.m_tStaticInfo.m_siUNo);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "MAXLOADCAP:%lu\n", pSecondList->m_up.m_tStaticInfo.m_uiMaxLoadCap);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "IP:%16.16s\n", pSecondList->m_up.m_tStaticInfo.m_szServiceIP);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "PORT:%u\n", pSecondList->m_up.m_tStaticInfo.m_usServicePort);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "CPUUSED:%u\n", pSecondList->m_up.m_tStateInfo.m_ucCPUPer);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "MEMUSED:%u\n", pSecondList->m_up.m_tStateInfo.m_ucMemPer);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "CURLINK:%u\n", pSecondList->m_up.m_tStateInfo.m_uiCurrLink);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "BALANCEVALUE:%u\n", pSecondList->m_usBalanceValue);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "TYPE:%c\n", pSecondList->m_cType);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "LINKNO:%u\n", pSecondList->m_uiLinkNo);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "SPEEDPORT:%u\n",pSecondList->m_up.m_tStaticInfo.m_usTestSpeedPort);
			pos += _snprintf(cmdbuf+pos, sizeof(cmdbuf)-pos, "AVAILABLE:%d\n\n", pSecondList->m_cIsAvaliable);
			ths->nsleep(10); util->ctl_cmd_print(cmd_no, cmdbuf);
		}
	}
	_snprintf(cmdbuf, sizeof(cmdbuf), 
		"总共有%d个服务器\n", totalServ);
	ths->nsleep(10); util->ctl_cmd_println(cmd_no, cmdbuf);
	return 0;
}

static int do_servinfo_cmd(
						   IN unsigned int cmd_no, IN char *argv[], IN int argc)
{
	futex_mutex_lock(&gLock);//加锁
	printDataLn(cmd_no, argv, argc);
	futex_mutex_unlock(&gLock);	//解锁
	return 0;
}

static int do_unknown_cmd(
						  IN unsigned int cmd_no, IN char *argv[], IN int argc)
{
	int					i, rc = 0;
	struct _util_ops	*util = &(mbi_sp_interface->util_ops);
	char				cmdbuf[MAX_PKG_BODY_SIZE];
	
	rc += _snprintf(cmdbuf + rc,
		sizeof(cmdbuf) - rc, "%s", "不支持的");
	
	for(i = 0; i < argc; i++)
		rc += _snprintf(cmdbuf + rc,
		sizeof(cmdbuf) - rc, " %s", argv[i]);
	
	rc += _snprintf(cmdbuf + rc,
		sizeof(cmdbuf) - rc, "%s", "命令");
	
	util->ctl_cmd_println(cmd_no, cmdbuf);

	
	return 0;
}

int	SPCALLBACK do_poise_ctl_cmd(
		IN unsigned int cmd_no, IN char *argv[], IN int argc)
{
	if(!strncmp(argv[0], "help", MAX_PKG_BODY_SIZE))
		return do_help_cmd(cmd_no, argv, argc);
	else if(!strncmp(argv[0], "version", MAX_PKG_BODY_SIZE))
		return do_version_cmd(cmd_no, argv, argc);
	else if(!strncmp(argv[0], "servinfo", MAX_PKG_BODY_SIZE))
		return do_servinfo_cmd(cmd_no, argv, argc);
 	else
 		return do_unknown_cmd(cmd_no, argv, argc);

	return 0;
}

char SPCALLBACK call_back_status(void)
{
	return 1;
}

int SPCALLBACK call_back_verbose_status(IN OUT char *verbose)
{	
	return 0;
}