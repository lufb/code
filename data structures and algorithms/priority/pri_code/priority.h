#ifndef _LUFUBO_PRIORITY_H_
#define _LUFUBO_PRIORITY_H_

/* 定义最大优先级级别*/
#define MAX_PRI_LEVEL	5

/* 存储优先级列表的数组长度 */
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
