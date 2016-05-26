#ifndef _OPT_FILE_SYSTEM_H_
#define _OPT_FILE_SYSTEM_H_

#include "OptDirMgr.h"
#include "chunk.h"


//忽略最低两位取地址
#define node_parent(r)		(unsigned long)((unsigned long)r & ~3)
//最低位置0
#define node_set0(r)		(unsigned long)((unsigned long)r & 0xFFFFFFFE)
//最低位置1
#define node_set1(r)    	(unsigned long)((unsigned long)r | 0x1)	
//最低位为0(是目录)
#define node_is0(r)			(!(r & 0x1))
//最低位为1(是普通文件)
#define node_is1(r)			(r & 0x1)

class MFileSystem{
public:
								MFileSystem();
								~MFileSystem();
	void						ReleaseRootNode();
	void						ReleaseBackup();
	void						ReleaseUsed();
	void						ReleaseFileNameANode(tagFileName* pFileNameNode);
	void						ReleaseNode(tagBasicNode *pNode, struct list_head *head);
	void						ReleaseABasicNode(tagBasicNode* pBasicNode);
	void						ReleaseFileName(tagFileName* pFileNameNode);
	void						ReleaseAdir(struct list_head *head);
	void						ReleaseARootNode(tagRootNode* root);
	int							InitRootNode();
	void						SetDirName(const char* dirName);
	void						SetWildCard(const char* wildCard);
	const char*					GetDirName();
	const char*					GetWildCard();
	int							Scan();
	bool						InnerMatch(const char* strSource, const char* strMatch);
	tagBasicNode*				InsertToRBTree( rb_root* root, tagBasicNode* node);
	tagBasicNode*				FindInRBTree(struct rb_root* root, tagBasicNode* findNode);
	tagBasicNode*				FindInRBTreeByName(char* name);
	int							DelInRBTree(rb_root* root, tagBasicNode* node);
	void						FileTimeToTime_t(  FILETIME  ft,  time_t  *t );
	int							GetAbsolutePath(tagBasicNode* pNode, char* path);
	int							Compare( tagBasicNode* lhp, tagBasicNode* rhp);
	int							CompareNodeAndName(char* path, tagBasicNode* rhp);
	int							FillUpFileNameNode(tagFileName *pNode, const char* pName);
	tagBasicNode*				GetABasicNode();
	tagFileName*				GetFileNameNode(size_t fileNameSize);
	tagBasicNode*				GetBasicNodeAddr(struct list_head *ptr);
	unsigned long				CalDirCrc(struct list_head* head);
	void						Swap();
	int							GetFileName(tagFileName *pNode, char* nameBuffer);

	unsigned char				GetSwitchTimes();	
	MCriticalSection&			GetCriticalSection();
	int							GetAttr(char* fileName, tagFileAttr& fileAttr);
	int							GetFileTotal();
	int							GetDirTotal();
	unsigned long				GetDirCrc();
	void						printDir();

	//私有成员函数
private:
	int							ScanADir(const char* pDir, const char* wildCard);
	int							_ScanADir(const char* pDir, const char* wildCard, const rb_root* root, struct list_head* head);
	int							_AddtoMem(tagBasicNode *pNode, const rb_root* root, struct list_head* head, WIN32_FIND_DATA& stFileInfo);



//私有成员变量
private:
	MCriticalSection			m_Section;
	char						m_cDirName[MAX_PATH];
	char						m_cWildCard[MAX_PATH];
	tagRootNode*				m_pRootNodeUsed;		//对外使用的，这里最好用指针，如若用变量，交换时会很麻烦
	tagRootNode*				m_pRootNodeBackup;		//扫描时临时的，这里最好用指针，如若用变量，交换时会很麻烦
	unsigned char				m_ucSwitchTimes;		//主备切换次数

#ifdef _DEBUG
public:	
	int							printADir(struct list_head* head);
#endif
};





#endif