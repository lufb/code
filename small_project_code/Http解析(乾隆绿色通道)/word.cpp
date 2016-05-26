#include "word.h"
#include <assert.h>
#include <stdio.h>

// void listAddTail(struct list_head *new1, struct list_head *head)
// {
// 	list_add_tail(new1, head);
// }


// void _insertWord(struct list_head *head, GC_WORD *word)
// {
// 	assert(word != NULL);
// 	listAddTail(&word->head, head);
// }

// void _initWord(struct list_head *head, GC_WORD *word, size_t wordCount)
// {
// 	for(size_t i = 0; i < wordCount; ++i)
// 	{
// 		_insertWord(head, word+i);
// 	}
// }

//从取左链表的第一个元素加到右链表的链表尾,成功返回0
// int pasteNode(struct list_head *lhs, struct list_head *rhs)
// {
// 	assert(lhs != NULL);
// 	assert(rhs != NULL);
// 
// 	struct list_head		*tmp;
// 	
// 	tmp = popQueue(lhs);
// 	
// 	if(NULL != tmp)
// 		return -1;
// 
// 	listAddTail(tmp, rhs);
// 
// 	return 0;
// }


void initWordMgr(WORD_MGR &wordMgr)
{
	wordMgr.wordCount = 0;

	for(size_t i = 0; i < sizeof(wordMgr.word)/sizeof(wordMgr.word[0]); ++i)
	{
		wordMgr.word[i].pWordBegin = NULL;
		wordMgr.word[i].size = 0;
	}

}


int isSeparator(char ch)
{
	//单词定界符定义（这里只针对GC的http协议解析）
	char delimiter[] = {' ', ':', '\n'};
	
	for(int i = 0; i < sizeof(delimiter)/sizeof(char); ++i)
	{
		if(ch == delimiter[i])
			return 1;
	}
	
	return 0;
}

int	isLineEnd(char ch)
{
	return ch == '\n';
}

//////////////////////////////////////////////////////////////////////////
// void print(struct list_head *header)
// {
// 	assert(header != NULL);
// 
// 	struct list_head				*tmp;
// 	GC_WORD							*pNode;
// 
// 	list_for_each(tmp, header)
// 	{
// 		pNode = list_entry(tmp, GC_WORD, head);
// 		printf("%d:%d\n", (long)(pNode->pWord), pNode->wordSize);
// 	}
// }

