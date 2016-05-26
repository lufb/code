/**
 *	include/bits/gcs_smif.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */
#ifndef	__BITS_SMIF_HEADER__
#define __BITS_SMIF_HEADER__

#define	GCS_MODULE_ID		0

/*	�����2��ö��ֵ,����write_log����ʹ��*/
enum {
	LOG_LEVEL_GENERAL		= 0,		/*	��ͨ��Ա����*/
	LOG_LEVEL_SUPPORT,					/*	ά����Ա����*/
	LOG_LEVEL_DEVELOPERS				/*	������Ա����*/
};

enum {
	LOG_TYPE_DEBUG			= 0,		/*	����*/
	LOG_TYPE_INFO,						/*	��Ϣ*/
	LOG_TYPE_WARN,						/*	����*/
	LOG_TYPE_ERROR,						/*	����*/
	LOG_TYPE_ALARM						/*	����*/
};

#define	_4X_PKG_SIZE_		8192		/*	4XЭ����Ĵ�С*/
#define	_4X_PKG_DATA_SIZE_	8000		/*	4XЭ���������Ĵ�С*/

#endif	/*	__BITS_SMIF_HEADER__*/