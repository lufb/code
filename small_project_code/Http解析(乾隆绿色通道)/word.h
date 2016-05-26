#ifndef _WORD_PARASE_H_
#define  _WORD_PARASE_H_
#include "list.h"
#include "stack.h"


#define	LINE_MAX_WORD_COUNT		16			//һ�����������󵥴�����



//�������ʵ���Ϣ�ṹ��
typedef struct _GC_WORD
{
	char						*pWord;
	size_t						wordSize;
	//����������û��head��
	//struct list_head			head;					
}GC_WORD;

//�������ʵĹ���
typedef struct _WORD_MGR
{
	//struct list_head		used;
	//struct list_head		free;
	//����ʹ��������ֻ��һ����������ԭ��1����Ч���Եͣ�2����Ҳ�Ե��鷳һЩ��3�ڴ�Ҳ��ԼЩ��
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