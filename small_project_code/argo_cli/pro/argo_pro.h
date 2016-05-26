#ifndef _ARGO_PRO_H_
#define _ARGO_PRO_H_

#define HASH_SIZE			20
#define FILE_NAME_SIZE		255 + 1		//	都带结尾符

#pragma pack(1)
//	协议头
typedef struct _argo_head{
	unsigned short		pro;			//	协议:	心跳包(0x00)/下发配置包(0x8001)/上传本地文件信息包(0x1)/更新的增量包(0x8002)/更新的全量包(0x8003)
	unsigned char		main;			//	
	unsigned char		child;
}argo_head;

//	对pro的定义
typedef enum
{
	PRO_CS_00 = 0x00,	//	客户端到服务器的心跳包
	PRO_CS_01 = 0x01,	//	客户端到服务器的文件信息包
	
	PRO_SC_O0 = 0x8000,	//	服务器到客户端的心跳包
	PRO_SC_OP = 0x8001,	//	服务器到客户端的配置包
	PRO_SC_AD = 0x8002,	//	服务器到客户端的增量包
	PRO_SC_AL = 0x8003,	//	服务器到客户端的全量包
}PRO_TYPE;


//	版本号
typedef struct _argo_base{
	unsigned short		heart_times;	//	多久没收到包时，发心跳包的时间（秒）
	unsigned short		recon_times;	//	发生错误时，重连的时间间隔（秒）
}argo_base;



//	服务器的配置信息
typedef struct _argo_hello{
	char				file_name[FILE_NAME_SIZE];	//	文件名(或目录名)带结尾符,且是不带目录分隔符
	unsigned int		file_type;					//	文件的类型(暂时只有普通文件，通配符文件)
	unsigned int		old_del;					//	对文件的操作(暂时只有通配符文件超过一定时间将其删除)		
}argo_hello;

typedef enum
{
	FILE_TYPE_MKTD = 0,
	FILE_FJY	= 1,
}ARGO_FILE_TYPE;

//	一	服务器下发配置及初始化信息
//	argo_head	|	argo_base|	argo_hello |	...	|	argo_hello



typedef struct _argo_world{
	char				file_name[FILE_NAME_SIZE];	//	普通文件名(如若是目录或通配符时，这里也是拆开成单独文件上传)
	unsigned char		file_hash[HASH_SIZE];		//	此文件的文件内容hash1值
}argo_world;

//	二	客户端上传本地文件信息
//	argo_head	|	argo_world	|	...	|	argo_world

//	全量包的头
typedef struct _argo_all_head{
	char				file_name[FILE_NAME_SIZE];	//	普通文件名(如若是目录或通配符时，这里也是拆开成单独文件上传)
	unsigned int		file_type;					//	00:实时文件，01:非实时文件
	//unsigned char		hash_old[HASH_SIZE];
	unsigned char		hash_new[HASH_SIZE];
	unsigned int		modify_time;				//	文件最后修改日期
	unsigned int		file_size;
}argo_all_head;

//	增量包的头
typedef struct _argo_up_head{
	char				file_name[FILE_NAME_SIZE];	//	普通文件名(如若是目录或通配符时，这里也是拆开成单独文件上传)
	unsigned int		file_type;					//	00:实时文件，01:非实时文件
	unsigned char		hash_old[HASH_SIZE];
	unsigned char		hash_new[HASH_SIZE];
	unsigned int		modify_time;				//	文件最后修改日期
	unsigned int		file_size;
}argo_up_head;

//	三	全量包
//	argo_head	|	argo_up_head	|	文件内容

typedef struct _argo_mask{
	unsigned int		size[9];				//每层的个数
}argo_mask;

typedef struct _bde_head{
	unsigned short		crc;
	unsigned short		size;
	unsigned char		specal	:	3;		/* bit(000)*/
	unsigned char		bde_type:	3;		//包类型：	3	bit(BDE)(000* , 001, 010, 011, 100, 101*, 110, 111), 其中有*的包类型不存在
	unsigned char		com_type:	2;		/* 00:	不压缩	01:zlib*/
	unsigned char		seq;				/* 包序号		*/
	unsigned char		main;				/* BDE主版本号	*/
	unsigned char		child;				/* BDE次版本号	*/
}bde_head;	

//	对bde_type的定义
typedef enum
{
	BDE_00	= 0,	//不存在这种包，拿来当开始状态
	BDE_E	= 1,
	BDE_D	= 2,
	BDE_DE	= 3,
	BDE_B	= 4,
	BDE_BE	= 5,		//约定不存在空包	
	BDE_BD	= 6,
	BDE_BDE	= 7,
}BDE_STATUS;

typedef struct _argo_dic{
	unsigned char		dic[16];
}argo_dic;

typedef struct _argo_dic_dc{
	void				swap(){
		unsigned int	*p = (unsigned int *)dic_data.dic;
		*p = ~(*p);
		p++;

		*p = ~(*p);
		p++;

		*p = ~(*p);
		p++;

		*p = ~(*p);
	}
	unsigned char		count;
	argo_dic			dic_data;
}argo_dic_dc;

/* */
//	四	增量包
/*
argo_head								|
argo_up_head							|
argo_mask								|
mask数据								|
字典（17字节）							|
变化数据的大小(原始大小)				|
变化的数据
*/

//头部定义(8字节, 64bit),不压
/*
CRC			:	16	bit		(除CRC外的该块的CRC)
该块大小	：	16	bit		(压缩后(如果有)body的大小)
特殊字符	：	3	bit(000)
包类型		：	3	bit(BDE)(000* , 001, 010, 011, 100, 101*, 110, 111)
压缩类型	：	2	bit	 (00:	不压缩	01:zlib)
块序号		:	8	bit	（00000000）
主版本号	:	8	bit	（针对此块的协议）
子版本号	：	8	bit	（针对此块的协议）
*/

//类型说明：
/*
B：	BEGIN包
D:	数据包
E:	结尾包

可能的包次序：

*/
#pragma pack()



#endif






	