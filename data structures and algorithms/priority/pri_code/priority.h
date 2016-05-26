#ifndef _LUFUBO_PRIORITY_H_
#define _LUFUBO_PRIORITY_H_

/* ����������ȼ�����*/
#define MAX_PRI_LEVEL	5

/* �洢���ȼ��б�����鳤�� */
#define PRI_LIST_SIZE	((1+MAX_PRI_LEVEL)*MAX_PRI_LEVEL/2)

class MPriority{
public:
							MPriority();
							~MPriority();

	void					PrList();
	int						GetPri();

private:
	void					_Init();

private:
	int						m_iPriList[PRI_LIST_SIZE];
	int						m_iCurIndex;
};


#endif
