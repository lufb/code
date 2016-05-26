#ifndef _OPT_FILE_SYSTEM_H_
#define _OPT_FILE_SYSTEM_H_

#include "../../common/chunk.h"
#include "../../common/OptStructDef.h"
#include "../../common/OptDirTranNetData.h"


//���������λȡ��ַ
#define node_parent(r)		(unsigned long)((unsigned long)r & ~3)
//���λ��0
#define node_set0(r)		(unsigned long)((unsigned long)r & 0xFFFFFFFE)
//���λ��1
#define node_set1(r)    	(unsigned long)((unsigned long)r | 0x1)	
//���λΪ0(��Ŀ¼)
#define node_is0(r)			(!(r & 0x1))
//���λΪ1(����ͨ�ļ�)
#define node_is1(r)			(r & 0x1)

//ȡ���λ
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

	
	
	//˽�г�Ա����
private:
	int							ScanADir(const char* pDir, const char* wildCard);
	int							_ScanADir(const char* pDir, const char* wildCard, const rb_root* root, struct list_head* head);
	int							_AddtoMem(tagBasicNode *pNode, const rb_root* root, struct list_head* head, WIN32_FIND_DATA& stFileInfo);
	int							_RemoteSync(char* path);	
	
	
	//˽�г�Ա����
private:
	char						m_cDirName[MAX_PATH];
	char						m_cWildCard[MAX_PATH];
	tagRootNode*				m_pRootNode;
	unsigned char				m_ucIndex;//���ļ�ϵͳ��Ӧ�ı��
	
#ifdef _DEBUG
public:	
	int							printADir(struct list_head* head);
	void						printDir();
#endif
};





#endif