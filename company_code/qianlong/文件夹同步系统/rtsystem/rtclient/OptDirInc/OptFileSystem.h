#ifndef _OPT_FILE_SYSTEM_H_
#define _OPT_FILE_SYSTEM_H_

#include "../../common/chunk.h"
#include "../../common/OptStructDef.h"
#include "../../common/OptDirTranNetData.h"


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

//取最低位
#define node_last_bit(r)	(unsigned long)((unsigned long) r & 0x1)


class MFileSystem{
public:
								MFileSystem();
								~MFileSystem();
	void						ReleaseRootNode();
	void						ReleaseBackup();
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
	int							DelInRBTree(rb_root* root, tagBasicNode* node);
	void						FileTimeToTime_t(  FILETIME  ft,  time_t  *t );
	int							GetAbsolutePath(tagBasicNode* pNode, char* path);
	int							Compare( tagBasicNode* lhp, tagBasicNode* rhp);
	int							FillUpFileNameNode(tagFileName *pNode, const char* pName);
	tagBasicNode*				GetABasicNode();
	tagFileName*				GetFileNameNode(size_t fileNameSize);
	tagBasicNode*				GetBasicNodeAddr(struct list_head *ptr);
	unsigned long				CalDirCrc(struct list_head* head);
	int							GetFileName(tagFileName *pNode, char* nameBuffer);
	void						SetIndex(unsigned char index);

	int							RemoteSync();
	int							GetABasicNodeAttr(char* path, tagNodeAttr* nodeAttr);
	int							GetRelativePath(struct list_head* head, char* path);

	int							RemoteOpenDir(char* path, tagResponseDirAttr* response);
	void						RequestFileAttrInit(char* path, tagRequestFileAttr& requestFile, tagResponseDirAttr& token);
	int							RemoteReadDir(tagRequestFileAttr& request, tagResponseFileAttr& response);
	int							RemoteOpenFile(tagRequestFileAttrInDisk& request, tagResponseFileAttrInDisk& response);
	int							RemoteReadFile(tagFileAttr& fileAttr, char* fileName);
	void						RemoteReadFileInit(tagFileAttr& fileAttr, char* fileName, tagRequestFileData& request);
	int							AdtoTmpList(struct list_head* head, tagResponseFileAttr& fileAttr);
	int							CompareNodeAndName(char* path, tagBasicNode* rhp);
	tagBasicNode*				FindInRBTreeByName(char* name);
	int							RemoteSyncAfile(tagFileAttr& fileAttr, char* fileName);
	int							FindChangedAndSync(char* path, struct list_head *head);
	int							FindInlist(struct list_head *head, char *dir, tagBasicNode* pNode);
	int							DelFile(char* fileName);
	int							DelDirTree(char* dir, int* delNum, int* delFileNum);
	int							CreateAEmptyDir(char* dir);
	void						ReturnListMemToMemPool(struct list_head* head);
	int							GetFileTotal();
	int							GetDirTotal();
	unsigned long				GetDirCrc();

	unsigned int				GetAddFile();
	unsigned int				GetChangeFile();
	unsigned int				GetDelFile();
	unsigned int				GetDelDir();
	unsigned int				GetAddDir();

	//tagBasicNode*				GetFirstChildNode(tagBasicNode* pNode);

	
	
	//私有成员函数
private:
	int							ScanADir(const char* pDir, const char* wildCard);
	int							_ScanADir(const char* pDir, const char* wildCard, const rb_root* root, struct list_head* head);
	int							_AddtoMem(tagBasicNode *pNode, const rb_root* root, struct list_head* head, WIN32_FIND_DATA& stFileInfo);
	int							_RemoteSync(char* path);	
	
	
	//私有成员变量
private:
	char						m_cDirName[MAX_PATH];
	char						m_cWildCard[MAX_PATH];
	tagRootNode*				m_pRootNode;
	unsigned char				m_ucIndex;//该文件系统对应的编号
	
#ifdef _DEBUG
public:	
	int							printADir(struct list_head* head);
	void						printDir();
#endif
};





#endif