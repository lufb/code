#ifndef _OPT_STRUCT_DEF_H_
#define _OPT_STRUCT_DEF_H_

#include "bits/list.h"
#include "rbtree.h"
#include "../../common/OptBaseDef.h"
#include <time.h>


//root根节点
typedef struct{
	//char					m_cName[MAX_PATH];		//根目录名
	struct list_head		head;					//根目录的链表头
	rb_root					root;					//红黑树root节点
	unsigned int			m_uiTotalDir;			//总目录数
	unsigned int			m_uiTotalFile;			//总文件数
	unsigned long			m_ulDirCrc;				//整个目录校验码
	
	unsigned int			m_uiAddFile;			//此次新增文件数
	unsigned int			m_uiChangeFile;			//此次改变文件数
	unsigned int			m_uiDelFile;			//此次删除文件数
	unsigned int			m_uiDelDir;				//此次删除目录数
	unsigned int			m_uiAddDir;				//此次新增目录数
} tagRootNode;

//存储一个文件的属性节点
typedef struct {
	time_t					m_ulLastModifyTime;		//文件最后修改时间
	unsigned long			m_ulFileSize;			//文件大小
} tagFileAttr;

//存储一个目录的属性节点
typedef struct {
	unsigned long			m_ulCRC32;				//目录校验码
	unsigned long			m_ulFileAndDirTotalNum;	//目录下文件和目录的总个数，不包括子目录
} tagDirAttr;

//存储一个文件或者目录的属性节点
typedef struct {
	union{
		tagFileAttr			m_sFileAttr;			//文件属性
		tagDirAttr			m_sDirAttr;			//目录属性
	};
} tagNodeAttr;

//文件名节点
typedef struct {
	char					m_cName[12];				//存储文件名
	unsigned long			m_ulNext;				//如果文件名被分割，指向下一个分割点，低两位存储引用计数
} tagFileName;

//构成文件系统的最基本节点
typedef struct {
	struct list_head		m_sBrother;				//链同一目录下的节点，即兄弟节点
	struct list_head 		m_sChildren;			//链表子目录节点，即子节点
	unsigned long			m_ulParant;				//指向父节点，指针低两位用以标识是文件还是目录
	struct rb_node			m_sRBNode;				//红黑树节点
	
	tagFileName 			*m_pName;				//指向文件名节点
	tagNodeAttr				m_sAttr;				//节点（文件或者）属性节点
} tagBasicNode;  //总共44字节





#endif