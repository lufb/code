#include "priority.h"
#include <stdio.h>
#include <assert.h>


MPriority::MPriority()
{
	m_iCurIndex = 0;
	_Init();
}

MPriority::~MPriority()
{

}



void
MPriority::_Init()
{
	int					i, j, k = 0;

	for(i = MAX_PRI_LEVEL; i > 0; --i){
		for(j = 0; j < i; ++j){
			if(k < PRI_LIST_SIZE)
				m_iPriList[k++] = MAX_PRI_LEVEL-j;
			else{
				assert(0);
				return;
			}
		}
	}
}

int
MPriority::GetPri()
{
	assert(m_iCurIndex <= PRI_LIST_SIZE);
	if(m_iCurIndex == PRI_LIST_SIZE)
		m_iCurIndex = 0;
	return m_iPriList[m_iCurIndex++];
}

void
MPriority::PrList()
{
	int					i;
	
	for(i = 0; i < PRI_LIST_SIZE; ++i)
		printf("%d ", m_iPriList[i]);
	
	printf("\n");
}

