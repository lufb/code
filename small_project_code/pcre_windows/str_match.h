#ifndef _STR_MSTCH_H_
#define _STR_MSTCH_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define PCRE_STATIC // ��̬�����ѡ��
#include "pcre.h"

#define OVECCOUNT 30


int
is_matched( const char *match_str,        /*  ������ʽƥ���   */
            const char *src_str)          /*  Դ�ַ���           */
{
    pcre            *re;
    const char      *error;
    int             erroffset;
    int             ovector[OVECCOUNT];
    int             rc;

    if(match_str == NULL)
        return 1;

    if(src_str == NULL)
        return 0;

    re = pcre_compile(  match_str,
                        0,
                        &error,
                        &erroffset,
                        NULL);
    if(re == NULL)
		return 0;

    rc = pcre_exec( re,
                    NULL,
                    src_str,
                    strlen(src_str),
                    0,
                    0,
                    ovector,
                    OVECCOUNT);
    pcre_free(re);

    return rc < 0 ? 0 : 1;
}

struct finded_str{
    unsigned int            begin;
    unsigned int            end;
};


/*
    ����ֵ:
    <   0   ����
    ==  0   û�ҵ�
    >   0   �ҵ��˼������
*/
int
find_str(   char *match_str,                /*  �����ҵ��ַ���  */
            char *str_src,                   /*  Դ�ַ���        */
            struct finded_str *result,       /*  �ҵ��Ľ��      */
            size_t  finded_str_count)        /*  ��Ž���ĳ���  */
{
    const char *error;
    int   erroffset;
    pcre *re;
    int   rc;
    int   i;
    int   ovector[1024];
    unsigned int findedcount = 0;
	unsigned int offset = 0;
	unsigned int len = strlen(str_src);

    if(match_str == NULL || str_src == NULL || result == NULL)
        return -1;

    re = pcre_compile (match_str,          /* the pattern */
                       PCRE_MULTILINE,
                       &error,         /* for error message */
                       &erroffset,     /* for error offset */
                       0);             /* use default character tables */
	if(!re)
		return -1;

    while (offset < len && 
        findedcount < finded_str_count-1 &&
        (rc = pcre_exec(re, 0, str_src, len, offset, 0, ovector, sizeof(ovector))) >= 0)
    {
        for( i = 0; i < rc; ++i)
        {
            result[findedcount].begin = ovector[2*i];
            result[findedcount++].end  = ovector[2*i+1];
        }
        offset = ovector[1];
    }

    free(re);
    return findedcount;
}

#endif


