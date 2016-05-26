#ifndef _CMD_H_
#define _CMD_H_

#include "lock.h"
#include "if.h"
#include "poise.h"
#include <assert.h>
#include "version.h"

extern struct mosp_interface	*mbi_sp_interface;
extern struct futex_mutex		gLock;
extern SRV_INFO					*gSrvInfo;
extern struct list_head			agentListHead;
extern struct list_head			updateListHead;			

int	SPCALLBACK do_poise_ctl_cmd(
		IN unsigned int cmd_no, IN char *argv[], IN int argc);

char SPCALLBACK call_back_status(void);

int SPCALLBACK call_back_verbose_status(IN OUT char *verbose);

static int do_help_cmd(
						  IN unsigned int cmd_no, IN char *argv[], IN int argc);

static int do_version_cmd(
						  IN unsigned int cmd_no, IN char *argv[], IN int argc);

static int printDataLn(
						   IN unsigned int cmd_no, IN char *argv[], IN int argc);

static int do_servinfo_cmd(
						   IN unsigned int cmd_no, IN char *argv[], IN int argc);

static int do_unknown_cmd(
						  IN unsigned int cmd_no, IN char *argv[], IN int argc);

int	SPCALLBACK do_poise_ctl_cmd(
						  IN unsigned int cmd_no, IN char *argv[], IN int argc);

char SPCALLBACK call_back_status(void);

int SPCALLBACK call_back_verbose_status(IN OUT char *verbose);
#endif
