#ifndef _OPT_DEAL_DIR_PROTOCOL_H_
#define _OPT_DEAL_DIR_PROTOCOL_H_

#include "../Global.h"
#include "../../common/OptDirTranNetData.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MDealProBase{
public:
	MDealProBase();
	virtual					~MDealProBase();
	//调虚函数的接口，它会去调相应的虚函数来处理	
	virtual int				DealProcess( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize ) = 0;
	//必须继承的接口和实现,共同部份
	int						SendFrame( unsigned long LinkNo, const tagComm_FrameHead * MainHead, unsigned char FrameCnt, unsigned char FrameNo, const void * Buf, unsigned short Size, bool bCompress = true ) const;
	int						SendErrorFrame( unsigned long LinkNo, const tagComm_FrameHead * MainHead, short shErrCode, const char* szErr = NULL ) const;
	int						Instance(tagSrvUnit_PlatInterface * pIn);
	tagBasicNode*			FindNode(unsigned char index, char* path);
	int						GetFileName(unsigned char index, tagFileName *pNode, char* tmpBuffer);
	void					InitFrameHead(tagComm_FrameHead& frameHead, tagSrvComm_LinkMsg * pLinkMsg);
private:
	tagCommFun_SendResponse*	m_SendResponse;					//发送应答消息
	tagCommFun_SendErrorMsg*	m_SendErrorMsg;					//发送错误消息
};

//230
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MDealPro_10_230: public MDealProBase{
public:
	MDealPro_10_230();
	~MDealPro_10_230();
	virtual	int				DealProcess( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize );
};

//231
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MDealPro_10_231: public MDealProBase{
public:
	MDealPro_10_231();
	~MDealPro_10_231();
	virtual	int				DealProcess( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize );
};

//232
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MDealPro_10_232: public MDealProBase{
public:
	MDealPro_10_232();
	~MDealPro_10_232();
	virtual	int				DealProcess( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize );
};

//233
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MDealPro_10_233: public MDealProBase{
public:
	MDealPro_10_233();
	~MDealPro_10_233();
	virtual	int				DealProcess( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize );
};

//234
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MDealPro_10_234: public MDealProBase{
public:
	MDealPro_10_234();
	~MDealPro_10_234();
	int						Check(tagRequestFileAttr*	pRequest, tagBasicNode*	pNode);
	virtual	int				DealProcess( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize );
};

//235
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MDealPro_10_235: public MDealProBase{
public:
	MDealPro_10_235();
	~MDealPro_10_235();
	virtual	int				DealProcess( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize );
	int						GetFileAttrInDisk(char* fileName, tagResponseFileAttrInDisk& response);
};

//236
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MDealPro_10_236: public MDealProBase{
public:
	MDealPro_10_236();
	~MDealPro_10_236();
	virtual	int				DealProcess( tagSrvComm_LinkMsg * pLinkMsg, char * pInBuf, unsigned short nInSize );
	int						GetFileData( const char* szFileName,unsigned long offset, unsigned long insize, char* out);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif