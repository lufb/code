#ifndef _WORD_PARASE_H_
#define  _WORD_PARASE_H_
#include "list.h"
#include "stack.h"


#define	LINE_MAX_WORD_COUNT		16			//一行中允许的最大单词数量



//描述单词的信息结构体
typedef struct _GC_WORD
{
	char						*pWord;
	size_t						wordSize;
	//不用链表，就没得head了
	//struct list_head			head;					
}GC_WORD;

//描述单词的管理
typedef struct _WORD_MGR
{
	//struct list_head		used;
	//struct list_head		free;
	//不再使用链表，而只用一个计数器（原因：1链表效率稍低，2操作也显得麻烦一些，3内存也节约些）
	size_t					wordCount;		
	WORD_STACK				word[LINE_MAX_WORD_COUNT];
} WORD_MGR;

//void				_insertWord(struct list_head *head, GC_WORD *word);
//void				_initWord(struct list_head *head, GC_WORD *word, size_t wordCount);
void				initWordMgr(WORD_MGR &worldMgr);
//void				listAddTail(struct list_head *new1, struct list_head *head);
//int					pasteNode(struct list_head *lhs, struct list_head *rhs);
int					isSeparator(char ch);
int					isLineEnd(char ch);



//////////////////////////////////////////////////////////////////////////
//void				print(struct list_head *head);




#endif