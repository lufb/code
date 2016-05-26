/**
 *	include/bits/gcs_cnt64.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */
#ifndef	__BITS_CNT64_HEADER__
#define __BITS_CNT64_HEADER__

#include "asm-x86/atomic.h"

/**
 *  64λ������
 */
typedef struct {
    atomic_t        low;                /*  ��32λ*/
    unsigned int    high;               /*  ��32λ*/
}cnt64_t;

#define CNT64_T_INC(_cnt_)                      \
    do {                                        \
        unsigned int    _addend;                \
        _addend = atomic_inc_return(            \
                        &((_cnt_)->low));       \
        if(_addend == 0)                        \
            (_cnt_)->high++;                    \
    } while(0)

#define CNT64_T_ADD(_cnt_, _a_)                 \
    do {                                        \
        unsigned int    _addend;                \
        _addend = atomic_add_return(            \
                    &((_cnt_)->low), (_a_));    \
        if(_addend < (unsigned int)(_a_))       \
            (_cnt_)->high++;                    \
    } while(0)

#endif	/*	__BITS_CNT64_HEADER__*/