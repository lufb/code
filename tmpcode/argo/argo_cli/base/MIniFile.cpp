//------------------------------------------------------------------------------------------------------------------------------
#include "MIniFile.h"
#include "MSystemInfo.h"
#ifdef LINUXCODE
#define stricmp strcasecmp
#include <string.h>
#endif
//------------------------------------------------------------------------------------------------------------------------------
MIniFile::MIniFile(void)
{
	m_lCurSection = -1;
	m_lCurNode = -1;

	m_bEditFlag = false;

	inner_addsection("");
}

//..............................................................................................................................
MIniFile::MIniFile( unsigned long NodeLimit ):m_mNode( NodeLimit ) 
{
	m_lCurSection = -1;
	m_lCurNode = -1;

	m_bEditFlag = false;

	inner_addsection("");
}

//..............................................................................................................................
MIniFile::~MIniFile()
{
	Close();
}
//..............................................................................................................................
int  MIniFile::inner_addsection(const char * szName)
{
	tagMIniFileSection				sttemp;
	register int					errorcode;

	my_strncpy(sttemp.szSectionName,szName,32);
	sttemp.lFirstChildSerial = -1;

	if ( (errorcode = m_mSection.Append(&sttemp)) < 0 )
	{
		return(errorcode);
	}

	m_lCurSection = errorcode;

	return(errorcode);
}
//..............................................................................................................................
int  MIniFile::inner_addnode(const char * szName,const char * szValue)
{
	tagMIniFileNode					sttemp;
	register int					errorcode;
	register int					iptr;

	if ( m_lCurSection < 0 )
	{
		assert(0);
		//throw exception("<MIniFile>��ǰSectionָ�뷢���쳣");
		return(ERR_PUBLIC_NOINSTANCE);
	}

	my_strncpy(sttemp.szNodeName,szName,32);
	my_strncpy(sttemp.szNodeValue,szValue,100);
	sttemp.lNextNodeSerial = -1;

	if ( (errorcode = m_mNode.Append(&sttemp)) < 0 )
	{
		return(errorcode);
	}

	iptr = m_mSection[m_lCurSection].lFirstChildSerial;
	if ( iptr < 0 )
	{
		//��һ���ڵ�
		m_mSection[m_lCurSection].lFirstChildSerial = errorcode;
	}
	else
	{
		//��N���ڵ�
		while ( m_mNode[iptr].lNextNodeSerial > 0 )
		{
			iptr = m_mNode[iptr].lNextNodeSerial;
		}

		m_mNode[iptr].lNextNodeSerial = errorcode;
	}

	m_lCurNode = errorcode;

	return(errorcode);
}
//..............................................................................................................................
int  MIniFile::inner_addsectiondescribe(const char * szDescribe)
{
	register int					errorcode;
	register int					istrlength;
	tagMIniFileDescribe				sttemp;

	if ( m_lCurSection < 0 )
	{
		assert(0);
		//throw exception("<MIniFile>��ǰSectionָ�뷢���쳣");
		return(ERR_PUBLIC_NOINSTANCE);
	}

	istrlength = strlen(szDescribe) + 1;
	if ( (errorcode = m_mDescribeContent.Append(szDescribe,istrlength)) < 0 )
	{
		return(errorcode);
	}

	sttemp.cType = 0x00;
	sttemp.lSectionNodeSerial = m_lCurSection;
	sttemp.lDescribeSerial = errorcode;

	if ( (errorcode = m_mDescribe.Append(&sttemp)) < 0 )
	{
		return(errorcode);
	}

	return(errorcode);
}
//..............................................................................................................................
int  MIniFile::inner_addnodedescribe(const char * szDescribe)
{
	register int					errorcode;
	register int					istrlength;
	tagMIniFileDescribe				sttemp;
	
	if ( m_lCurNode < 0 )
	{
		assert(0);
		//throw exception("<MIniFile>��ǰSectionָ�뷢���쳣");
		return(ERR_PUBLIC_NOINSTANCE);
	}
	
	istrlength = strlen(szDescribe) + 1;
	if ( (errorcode = m_mDescribeContent.Append(szDescribe,istrlength)) < 0 )
	{
		return(errorcode);
	}
	
	sttemp.cType = 0x01;
	sttemp.lSectionNodeSerial = m_lCurNode;
	sttemp.lDescribeSerial = errorcode;
	
	if ( (errorcode = m_mDescribe.Append(&sttemp)) < 0 )
	{
		return(errorcode);
	}
	
	return(errorcode);
}
//..............................................................................................................................
int  MIniFile::inner_readsection(const char * strIn)
{
	MString						strsectionname;
	MString						strtemp;
	MString						strdescribe;
	register int				errorcode;

	if ( (errorcode = MString(strIn + 1).GetMatchString("]",&strsectionname,&strtemp)) < 0 )
	{
		//û��ƥ���]��־
		return(errorcode);
	}

	if ( (strdescribe = strtemp.GetMatchBackString(";")) == "" )
	{
		strdescribe = strtemp.GetMatchBackString("��");
	}

	strsectionname.TrimLeft();
	strsectionname.TrimRight();

	strdescribe.TrimLeft();
	strdescribe.TrimRight();

	if ( strsectionname != "" )
	{
		if ( (errorcode = inner_addsection(strsectionname.c_str())) < 0 )
		{
			return(errorcode);
		}

		if ( strdescribe != "" )
		{
			if ( (errorcode = inner_addsectiondescribe(strdescribe.c_str())) < 0 )
			{
				return(errorcode);
			}
		}
	}

	return(1);
}

//..............................................................................................................................
int  MIniFile::inner_readnode2(const char * strIn)
{
	char sBuf[100];
	//add by liuqy 20100805 for �Ⱥź����Ч����֮��Ŀո�ɾ��
	long lFlag = 0;
	long lLen;
	char * pszValue = NULL;
	char * pszName;
	memset(sBuf, 0, sizeof(sBuf));
	lLen = strlen(strIn);
	if(lLen >= sizeof(sBuf))
		lLen = sizeof(sBuf) - 1;
	memcpy(sBuf, strIn, lLen);
	
	for(register int i = 0; i < lLen; i++)
	{

		//modify by liuqy 20100805 for �Ⱥź����Ч����֮��Ŀո�ɾ��
		if(' ' != sBuf[i] && 0 == lFlag )
		{
			lFlag = 1;
			pszName = &sBuf[i];
		}
		if(' ' != sBuf[i] && 2 == lFlag )
		{
			lFlag = 3;
			pszValue = &sBuf[i];
		}
		if('=' == sBuf[i])
		{
			lFlag = 2;
			sBuf[i] = 0;
		}
		if((sBuf[i] == ' ' && 3 == lFlag) || sBuf[i] == 10 || sBuf[i] == ';' || sBuf[i] == 13)
		{
			sBuf[i] = 0;
			break;
		} 
		if(' ' == sBuf[i])
			sBuf[i] = 0;
		//end modify
	}
	if(3 <= lFlag)
	{
		return(inner_addnode(pszName, pszValue));
	}
	else
	{
		return inner_addnode(sBuf, "");
	}

	return(1);
}

//..............................................................................................................................
int  MIniFile::inner_readnode(const char * strIn)
{
	MString						strnodename;
	MString						strtemp;
	MString						strvalue;
	MString						strdescribe;
	register int				errorcode;

	if ( (errorcode = MString(strIn).GetMatchString("=",&strnodename,&strtemp)) < 0 )
	{
		//û��=��־
		return(errorcode);
	}

	if ( (errorcode = strtemp.GetMatchString(";",&strvalue,&strdescribe)) < 0 )
	{
		if ( (errorcode = strtemp.GetMatchString("��",&strvalue,&strdescribe)) < 0 )
		{
			//û��ע����Ϣ
			strnodename.TrimLeft();
			strnodename.TrimRight();
			
			strtemp.TrimLeft();
			strtemp.TrimRight();


			if ( strnodename != "" )
			{
				return(inner_addnode(strnodename.c_str(),strtemp.c_str()));
			}
		}
	}

	//��ע����Ϣ
	strnodename.TrimLeft();
	strnodename.TrimRight();
	
	strvalue.TrimLeft();
	strvalue.TrimRight();

	strdescribe.TrimLeft();
	strdescribe.TrimRight();
	
	if ( strnodename != "" )
	{
		if ( (errorcode = inner_addnode(strnodename.c_str(),strvalue.c_str())) < 0 )
		{
			return(errorcode);
		}

		if ( strdescribe != "" )
		{
			if ( (errorcode = inner_addnodedescribe(strdescribe.c_str())) < 0 )
			{
				return(errorcode);
			}
		}
	}

	return(1);
}
//..............................................................................................................................
int  MIniFile::Open(MString strFileName, int nDh)
{
	MFile						mfileptr;
	register int				errorcode;
	char						tempbuf[512];
	unsigned char				clasttype = 0x00;
	
	if ( strFileName.GetMatchBackString("\\") == "" && strFileName.GetMatchBackString("/") == "" )
	{
		//û������·����ֻ���ļ�����
		m_strFileName = MSystemInfo::GetApplicationPath() + strFileName;
	}
	else
	{
		m_strFileName = strFileName;
	}

	if ( (errorcode = mfileptr.OpenRead(m_strFileName)) < 0 )
	{
		//���ļ����󣬹ر�ʱ��Ҫд���ļ���
		m_bEditFlag = true;

		return(errorcode);
	}

	mfileptr.Seek(0,0);

	while ( mfileptr.ReadString(tempbuf,512) > 0 )
	{
		//����ո�TAB��
		MString::TrimLeft(tempbuf);

		//����
		if ( tempbuf[0] == 0x00 )
		{
			continue;
		}
		//��ȡSection
		else if ( tempbuf[0] == '[' )
		{
			inner_readsection(tempbuf);
			clasttype = 0x00;
		}
		//��ȡע��
		else if ( tempbuf[0] == ';' )
		{
			MString::TrimRight(tempbuf + 1);

			if ( clasttype == 0x00 )
			{
				inner_addsectiondescribe(tempbuf + 1);
			}
			else
			{
				inner_addnodedescribe(tempbuf + 1);
			}
		}
		else if ( memcmp(tempbuf,"��",2) == 0 )
		{
			MString::TrimRight(tempbuf + 2);

			if ( clasttype == 0x00 )
			{
				inner_addsectiondescribe(tempbuf + 2);
			}
			else
			{
				inner_addnodedescribe(tempbuf + 2);
			}
		}
		//��ȡNode
		else
		{
			//�����Node�еȺ�
			if(nDh == 1)
			{
				inner_readnode(tempbuf);
			}else{
				//�����Nodeֻ�����ƣ�û�еȺź�ֵ
				inner_readnode2(tempbuf);
			}
			clasttype = 0x01;
		}
	}
	
	mfileptr.Close();

	//���ļ��ɹ����ر�ʱ����Ҫд���ļ���
	m_bEditFlag = false;

	return(1);
}

//..............................................................................................................................
int  MIniFile::inner_autospace(MFile * lpFilePtr,const char * strIn)
{
	char							tempbuf[50];
	register int					isize;

	memset(tempbuf,' ',50);
	if ( (isize = 50 - strlen(strIn)) <= 0 )
	{
		//modify by liuqy 20100430 ���ٻ�����Ҫд��һ���ո�,���û��һ������ע�⣬��ע����ֵ��Ϊһ���ˣ����ÿ�����
		isize = 1;
		//return(1);
		//end modify
	}

	lpFilePtr->Write(tempbuf,isize);

	return(1);
}

int  MIniFile::WriteAndClose(void)
{
	register int					inodeptr;
	MFile							mfileptr;
	register int					errorcode;
	char							tempbuf[512];
	register int					isectionptr;

	if ( m_bEditFlag == true )
	{
		m_bEditFlag = false;

		MFile::DeleteFile(m_strFileName);
		if ( (errorcode = mfileptr.Create(m_strFileName)) < 0 )
		{
			return(errorcode);
		}
	}
	
	if(m_bEditFlag == false){
		//��ʼд���ļ�
		for ( isectionptr = 0;isectionptr<m_mSection.GetCount();isectionptr ++)
		{
			//д��Section
			if ( isectionptr != 0 )
			{
				memset(tempbuf, 0, sizeof(tempbuf));
				sprintf(tempbuf, "[%s]", m_mSection[isectionptr].szSectionName);
				mfileptr.Write(tempbuf,strlen(tempbuf));

			}

			mfileptr.Write("\r\n",2);

			//д��Node
			inodeptr = m_mSection[isectionptr].lFirstChildSerial;
			while ( inodeptr >= 0 )
			{
				//д��Node
				memset(tempbuf, 0, sizeof(tempbuf));
				sprintf(tempbuf, "%s", m_mNode[inodeptr].szNodeName);
				mfileptr.Write(tempbuf,strlen(tempbuf));
				mfileptr.Write("\r\n",2);

				inodeptr = m_mNode[inodeptr].lNextNodeSerial;	
			}
		}
	}
	
	mfileptr.Close();
	return(1);
}
//..............................................................................................................................
int  MIniFile::inner_writefile(void)
{
	register int					isectionptr;
	register int					inodeptr;
	MFile							mfileptr;
	register int					errorcode;
	char							tempbuf[512];
	register int					i;
	bool							mwriteln;

	if ( m_bEditFlag == true )
	{
		m_bEditFlag = false;

		MFile::DeleteFile(m_strFileName);
		if ( (errorcode = mfileptr.Create(m_strFileName)) < 0 )
		{
			return(errorcode);
		}

		//��ʼд���ļ�
		for ( isectionptr = 0;isectionptr<m_mSection.GetCount();isectionptr ++)
		{
			//д��Section
			//modify by liuqy 20100430 for �ڵ��������Ҫд��
			//if ( isectionptr != 0 )
			if(0 < strlen(m_mSection[isectionptr].szSectionName))
			//end modify
			{
				my_snprintf(tempbuf,512,"\r\n[%s]",m_mSection[isectionptr].szSectionName);
				mfileptr.Write(tempbuf,strlen(tempbuf));
				inner_autospace(&mfileptr,tempbuf);
				mwriteln = true;
			}

			//д��Section ע��
			for ( i=0;i<m_mDescribe.GetCount();i++ )
			{
				if ( m_mDescribe[i].cType == 0x00 && m_mDescribe[i].lSectionNodeSerial == isectionptr )
				{
					my_snprintf(tempbuf,512,";%s\r\n",&m_mDescribeContent[m_mDescribe[i].lDescribeSerial]);
					mfileptr.Write(tempbuf,strlen(tempbuf));
					mwriteln = false;
				}
			}

			if (  mwriteln == true )
			{
				mfileptr.Write("\r\n",2);
			}

			//д��Node
			inodeptr = m_mSection[isectionptr].lFirstChildSerial;
			while ( inodeptr >= 0 )
			{
				//д��Node
				my_snprintf(tempbuf,512,"%s = %s",m_mNode[inodeptr].szNodeName,m_mNode[inodeptr].szNodeValue);
				mfileptr.Write(tempbuf,strlen(tempbuf));
				inner_autospace(&mfileptr,tempbuf);

				//д��Node ע��
				mwriteln = true;
				for ( i=0;i<m_mDescribe.GetCount();i++ )
				{
					if ( m_mDescribe[i].cType == 0x01 && m_mDescribe[i].lSectionNodeSerial == inodeptr )
					{
						my_snprintf(tempbuf,512,";%s\r\n",&m_mDescribeContent[m_mDescribe[i].lDescribeSerial]);
						mfileptr.Write(tempbuf,strlen(tempbuf));
						mwriteln = false;
					}
				}

				if ( mwriteln == true )
				{
					mfileptr.Write("\r\n",2);
				}

				inodeptr = m_mNode[inodeptr].lNextNodeSerial;
				
			}
		}


		mfileptr.Close();
	}

	return(1);
}
//..............................................................................................................................
void MIniFile::Close(void)
{
	inner_writefile();
	m_mSection.Clear();
	m_mNode.Clear();
	m_mDescribe.Clear();
	m_mDescribeContent.Clear();

	m_lCurSection = -1;
	m_lCurNode = -1;

	m_bEditFlag = false;

	inner_addsection("");
}
//..............................................................................................................................
MString MIniFile::ReadString(MString strSection,MString strNode,MString strDefault)
{
	register int					i;
	register int					inodeptr = -1;

	//����Section
	for ( i=0;i<m_mSection.GetCount();i++ )
	{
		if ( stricmp(strSection.c_str(),m_mSection[i].szSectionName) == 0 )
		{
			inodeptr = m_mSection[i].lFirstChildSerial;
			break;
		}
	}

	//����Node
	while ( inodeptr >= 0 )
	{
		if ( stricmp(strNode.c_str(),m_mNode[inodeptr].szNodeName) == 0 )
		{
			return(m_mNode[inodeptr].szNodeValue);
		}

		inodeptr = m_mNode[inodeptr].lNextNodeSerial;
	}

	return(strDefault);
}
//..............................................................................................................................
int  MIniFile::ReadInteger(MString strSection,MString strNode,int iDefault)
{
	MString							strtemp;

	strtemp = ReadString(strSection,strNode,MString(iDefault));

	return(strtemp.ToLong());
}
//..............................................................................................................................
double MIniFile::ReadDouble(MString strSection,MString strNode,double dDefault)
{
	MString							strtemp;
	
	strtemp = ReadString(strSection,strNode,MString(dDefault));
	
	return(strtemp.ToDouble());
}
//..............................................................................................................................
bool MIniFile::ReadBool(MString strSection,MString strNode,bool bDefault)
{
	MString							strtemp;
	
	strtemp = ReadString(strSection,strNode,MString(bDefault));
	
	return(strtemp.ToBool());
}
//..............................................................................................................................
int  MIniFile::WriteString(MString strSection,MString strNode,MString strValue,MString strDescribe)
{
	register int					i;
	register int					isectionptr = -1;
	register int					inodeptr = -1;
	register int					idescribeptr = -1;
	register int					errorcode;
	
	//����Section
	for ( i=0;i<m_mSection.GetCount();i++ )
	{
		//add by liuqy 20100429 for ��Ҫ���Դ�Сд
		if (0 == stricmp(strSection.c_str(), m_mSection[i].szSectionName) )
		{
			isectionptr = i;
			inodeptr = m_mSection[i].lFirstChildSerial;
			break;
		}
	}

	//û�ж�Ӧ��Section����Ҫ����һ��
	if ( isectionptr == -1 )
	{
		if ( (errorcode = inner_addsection(strSection.c_str())) < 0 )
		{
			//���Section��������
			return(errorcode);
		}

		isectionptr = errorcode;
	}

	m_lCurSection = isectionptr;

	//����Node
	while ( inodeptr >= 0 )
	{
		//add by liuqy 20100429 for ��Ҫ���Դ�Сд
		if (0 == stricmp( strNode.c_str(), m_mNode[inodeptr].szNodeName ))
		{
			my_snprintf(m_mNode[inodeptr].szNodeValue,100,"%s",strValue.c_str());
			//add by liuqy 20100429 ��ԭ�������޸�������û���ҵ�ԭ��������������Ҫ����
			if(strDescribe != "")
			{
				m_lCurNode = inodeptr;
				for ( i=0;i<m_mDescribe.GetCount();i++ )
				{
					if ( m_mDescribe[i].cType == 0x01 && m_mDescribe[i].lSectionNodeSerial == inodeptr )
					{
						m_lCurNode = -1;
						//��λ�ñ��棬��ֱ�ӱ���,�ռ䲻��ʱ����׷��һ��
						if(strlen(&m_mDescribeContent[m_mDescribe[i].lDescribeSerial]) >= strDescribe.GetLength())
							strcpy(&m_mDescribeContent[m_mDescribe[i].lDescribeSerial], strDescribe.c_str());
						else
						{
							if ( (errorcode = m_mDescribeContent.Append(strDescribe.c_str(), strDescribe.GetLength()+1)) < 0 )
							{
								return(errorcode);
							}
							m_mDescribe[i].lDescribeSerial = errorcode;

						}
						break;
					}
				}
			}
			//end add 
			break;
		}
		
		inodeptr = m_mNode[inodeptr].lNextNodeSerial;
	}

	//û�ж�Ӧ��Node,��Ҫ���һ��
	if ( inodeptr < 0 )
	{
		if ( (inodeptr = inner_addnode(strNode.c_str(),strValue.c_str())) < 0 )
		{
			//д�����ݷ�������
			return(inodeptr);
		}
	}

	//д��Describe //modify by liuqy 20100429 for û�нڵ�λ�þͲ���������
	if ( strDescribe != "" && 0 <= m_lCurNode )
	{
		if ( (idescribeptr = inner_addnodedescribe(strDescribe.c_str())) < 0 )
		{
			//д��Describe��������
			return(idescribeptr);
		}
	}

	m_bEditFlag = true;

	return(1);
}
//..............................................................................................................................
int  MIniFile::WriteInteger(MString strSection,MString strNode,int iValue,MString strDescribe)
{
	return(WriteString(strSection,strNode,MString(iValue),strDescribe));
}
//..............................................................................................................................
int  MIniFile::WriteDouble(MString strSection,MString strNode,double dValue,MString strDescribe)
{
	return(WriteString(strSection,strNode,MString(dValue),strDescribe));
}
//..............................................................................................................................
int  MIniFile::WriteBool(MString strSection,MString strNode,bool bValue,MString strDescribe)
{
	return(WriteString(strSection,strNode,MString(bValue),strDescribe));
}
//..............................................................................................................................
int  MIniFile::GetSectionCount(void)
{
	return(m_mSection.GetCount());
}
//..............................................................................................................................
MString MIniFile::GetSectionName(unsigned long lSerial)
{
	if ( lSerial >= m_mSection.GetCount() )
	{
		return("");
	}

	return(m_mSection[lSerial].szSectionName);
}
//------------------------------------------------------------------------------------------------------------------------------

/***********************************************************************
 *	��������rl_ReadSection
 *	���ܣ���ȡָ��Ӧ���µ����нڵ�
 *	������in_strSection			����-Ӧ������
 *		out_ppszNodeNameList	���-�ڵ�����ָ������
 *		out_ppszNodeValList		���-�ڵ�ֵָ������
 *		in_lNodeNameValListSize	����-�ڵ����ƺ�ֵ���������С
 *	���أ���ȡ���ݵĸ�����0��ʾ�ޣ�-1��ʾ����
 *	��ʷ��liuqy 20100426 create
 ***********************************************************************/
long MIniFile::rl_ReadSection(MString in_strSection, char ** out_ppszNodeNameList, char ** out_ppszNodeValList, long in_lNodeNameValListSize)
{
	register int					i;
	register int					inodeptr = -1;
	long	lCount;

	if(NULL == out_ppszNodeNameList || NULL == out_ppszNodeValList || 0 >= in_lNodeNameValListSize)
	{
		return -1;
	}
	//����Section
	for ( i=0;i<m_mSection.GetCount();i++ )
	{
		if ( stricmp(in_strSection.c_str(), m_mSection[i].szSectionName) == 0 )
		{
			inodeptr = m_mSection[i].lFirstChildSerial;
			break;
		}
	}
	lCount = 0;
	//����Node
	while ( inodeptr >= 0 )
	{
		out_ppszNodeNameList[lCount] = m_mNode[inodeptr].szNodeName;
		out_ppszNodeValList[lCount] = m_mNode[inodeptr].szNodeValue;
		lCount++;
		if(in_lNodeNameValListSize <= lCount)
			break;

		inodeptr = m_mNode[inodeptr].lNextNodeSerial;
	}

	return(lCount);
	
}
