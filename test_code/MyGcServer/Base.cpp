#include "StdAfx.h"
#include "Base.h"
#include <assert.h>
#include "MHttp.h"

// int myRecv(int s, void *buf, size_t len)
// {
// 	assert(buf != NULL);
// 
// 	int iRecvTotal = 0;     
// 	int iRecvOnce = 0;     
// 	while ( iRecvTotal < len )        
// 	{         
// 		iRecvOnce = ::recv( s, (char* )buf + iRecvTotal, len - iRecvTotal, 0 );         
// 		if ( iRecvOnce == SOCKET_ERROR || iRecvOnce == 0 )         
// 		{     
// 			return -1;         
// 		}         
// 		iRecvTotal += iRecvOnce;    
// 	} 
// 	
// 	return iRecvTotal;
// 	
// }

int	mySend(int s, void *buffer, size_t size)
{
	assert(buffer != NULL);
	int rc;
	
	rc = send(s, (char *)buffer, size, 0);
	if(rc <= 0)	
		return GetLastError();
	
	return rc;
}

int myHttpSend(int s,  char *buffer, size_t size, THREAD_PARAM *param)
{
	char				tmpBuffer[10240];
	int					errCode;

	errCode = MHttp::BuildData(tmpBuffer, sizeof(tmpBuffer), param->usUnitSerial, param->ulLinkNo, buffer, size);
	
	if(mySend(s, tmpBuffer, errCode) != errCode)
	{
		printf("httpSend Error\n");
		return -1;
	}

	return size;
}