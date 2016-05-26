/**
 *	include/chunk.h
 *
 *	Copyright (C) 2012 ���� <yl.tienon@gmail.com>
 *
 *	�������chunk��������Ĵ���ӿ�
 *
 *	�޸���ʷ:
 *
 *	2012-09-18 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef		__CHUNK_HEADER__
#define		__CHUNK_HEADER__

#include "bits/chunk.h"

#ifdef  __cplusplus
extern  "C" {
#endif

/**
 *	chunk_mgr_init - ��ʼ��һ��chunk���������
 *
 *	@spc:		size_per_chunk ÿ��chunk�Ĵ�С(Bytes)
 *	@spb:		size_per_block ÿ��chunk�зֿ�Ĵ�С(Bytes)
 *
 *	return
 *		NULL	ʧ��
 *		!NULL	chunk���������
 */
struct chunks_mgr*	chunk_mgr_init(size_t spc, size_t spb);

/**
 *	chunk_mgr_destroy - ����һ��chunk���������
 *
 *	@mgr:			chunk���������
 *
 *	return
 *		��
 */
void	chunk_mgr_destroy(struct chunks_mgr *mgr);

/**
 *	chunk_mgr_truncate - ���һ��chunk����������µ�������
 *
 *	@mgr:			chunk���������
 *
 *	return
 *		��
 *
 *	remark:
 *		ע����ñ�������ʱ��,��ȷ��û���κ��߳�������
 *	��chunk����������µ�������
 */
void	chunk_mgr_truncate(struct chunks_mgr *mgr);

/**
 *	alloc_block - ����һ�����ݿ�
 *
 *	@mgr:		chunk���������
 *
 *	return
 *		NULL	ʧ��(�ڴ治��,���߳�����������chunk����)
 *		!NULL	block�����ݻ�����	
 */
void*	alloc_block(struct chunks_mgr *mgr);

/**
 *	free_block - �ͷ�һ�����ݿ�
 *
 *	@mgr:		chunk���������
 *
 *	return
 *		��
 */
void	free_block(struct chunks_mgr *mgr, void *block);

#ifdef __cplusplus
}
#endif

#endif		/*	__CHUNK_HEADER__*/
