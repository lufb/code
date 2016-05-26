#ifndef _OPTION_H_
#define _OPTION_H_

#include <memory.h>

#define MAX_IP_LEN			256			/* IP最大长度 */
#define MAX_OPTION			32			/* 配置最大项数 */


typedef struct _option_record_{
	unsigned short			us_lisport;
	unsigned short			us_dstport;
	char					c_dstip[MAX_IP_LEN];
	_option_record_(){
		us_dstport			= (unsigned short)-1;
		us_dstport			= (unsigned short)-1;
		memset(c_dstip, 0, sizeof(c_dstip));
	}
}OPTION_RECORD;

class MOption{
public:
							MOption();
							~MOption();
	int						init(int argc, char *argv[]);

private:
	OPTION_RECORD			m_optdata[MAX_OPTION];
	int						optioncount;
};


#endif