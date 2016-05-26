//------------------------------------------------------------------------------------------------------------------------------
#include "MTypeDefine.h"
//------------------------------------------------------------------------------------------------------------------------------
char * my_strncpy(char * strDest,const char * strSource,size_t iCount)
{
	char				*	lpsztempptr;

	assert(strDest != NULL);
	assert(strSource != NULL);
	assert(iCount > 0);

	if ( strDest == NULL || strSource == NULL || iCount <= 0 )
	{
		return(NULL);
	}

	lpsztempptr = strncpy(strDest,strSource,iCount);
	strDest[iCount - 1] = 0;
	
	return(lpsztempptr);
}
//..............................................................................................................................
char * my_strncat(char * strDest,const char * strSource,size_t iCount)
{
	char				*	lpsztempptr;
	
	assert(strDest != NULL);
	assert(strSource != NULL);
	assert(iCount > 0);
	
	if ( strDest == NULL || strSource == NULL || iCount <= 0 )
	{
		return(NULL);
	}

	lpsztempptr = strncat(strDest,strSource,iCount);
	strDest[iCount - 1] = 0;
	
	return(lpsztempptr);
}
//..............................................................................................................................
void my_snprintf(char * lpszBuffer,size_t lCount,const char * lpszFormat,...)
{
	va_list					smarker;

	assert(lpszBuffer != NULL);
	assert(lCount > 0);

	if ( lpszBuffer == NULL || lCount <= 0 )
	{
		return;
	}
	
	va_start(smarker,lpszFormat);
	_vsnprintf(lpszBuffer,lCount,lpszFormat,smarker);
	va_end( smarker );

	lpszBuffer[lCount - 1] = 0;
}
//------------------------------------------------------------------------------------------------------------------------------
