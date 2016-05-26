/*
 *	include/asm-x86/atomic.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	����x86��ϵ�ṹ��ԭ�Ӳ���һЩ����
 *
 *	���κδ�����ƽ̨��,������һЩԭ���Բ���,�����������ǳ���ʹ��
 *	���ǿ�ϧ�����������ͨ��������C���Եķ�ʽ���ṩ,���˵�������
 *	
 *	WINDOWSƽ̨�¿���ʹ��InterlockedFunction�庯��ʵ��ԭ�Ӳ���
 *	����Ҫ���Ƕ�CPU����������InterlockedFunction�庯�����
 *	
 *	��LINUXƽ̨���ǿ���ʹ��Ƕ��ʽ���ʵ��ԭ�ӵĲ���,ֵ��ע�����
 *	LINUX�±����ʱ�����ȷ��Ŀ�����Ҫ���еĻ����Ƿ��Ƕദ�����
 *	�ڵ������������,ÿ��ָ���ִ�ж���ԭ���Ե�
 *	���ڶദ���������ֻ����Щ�����Ķ�������д��������ԭ���Ե�.
 *	Ϊ���ֲ���һȱ��,x86���ָ���ṩ�˸��ӵ�lockǰ׺,
 *	ʹ��lockǰ׺�Ķ��޸�дָ��Ҳ��ԭ����ִ�� ��lockǰ׺��ָ���ڲ���ʱ����ס����
 *	ʹ�����ִ�м�ʹ�ڶദ������Ҳ��ԭ����ִ�е� 
 *	�Ҳο���CPU��ָ��˵��,�����ᵽxchgָ���lockǰ׺Ҳ��ԭ����ִ��
 *	Ҳ����˵xchgִ��ʱĬ�ϻ����ڴ�����.
 *	����Ϊ�˰�ȫ�ڼ�,�һ�����xchgָ�����LOCKǰ׺��.������Ҫ�����ڶ�CPU�Ļ�
 *	LINUX�ı�����Ҫ���� -D CONFIG_SMP��������
 *
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef	__ARCH_ATOMIC_I386_HEADER__
#define	__ARCH_ATOMIC_I386_HEADER__

#include	"features.h"

#ifdef WIN32
#else
#ifdef	CONFIG_SMP						/*	�Գƶ�CPU��ǰ׺*/
#define LOCK "lock ; "
#else
#define LOCK ""
#endif
#endif

/*	����I386��ԭ�Ӳ�������*/
#ifdef WIN32
typedef struct {ULONG volatile counter;} atomic_t;
#else
typedef struct {volatile int counter;} atomic_t;
#endif

/**
 *	atomic_set - ԭ�����ñ���
 *
 *	@v:		ָ��atomic_t���͵�ָ��
 *
 *	remark:
 *		����ԭ�Ӷ�����&vֵ(����λ������)
 */
#define atomic_read(v)      ((v)->counter)

#ifdef  __cplusplus
extern  "C" {
#endif

/**
 *	atomic_set - ԭ�����ñ���
 *
 *	@v:		ָ��atomic_t���͵�ָ��
 *	@i:		�����ֵ
 *
 *	remark:
 *		ԭ�ӵ�����ֵ @i --> @v (����λ������)
 */
static INLINE_FN void atomic_set(atomic_t *v, int i)
{
#ifdef WIN32
	InterlockedExchange((LPLONG)(&(v->counter)), i);
#else
	v->counter	=	i;
#endif
}

/**
 *	atomic_add - ԭ�ӵĽ�һ��atomic_t���ͱ�������һ��ָ����32λ������ֵ
 *
 *	@v:		ָ��atomic_t���͵�ָ��
 *	@i:		�����ֵ
 *
 *	remark:
 *		ԭ�ӵĽ�@v �� @i(����λ������)
 */
static INLINE_FN void atomic_add(atomic_t *v, int i)
{
#ifdef WIN32
	InterlockedExchangeAdd((LPLONG)(&(v->counter)), i);
#else
	 __asm__ __volatile__(
		LOCK "addl %1,%0"
		:"=m" (v->counter)
		:"ir" (i), "m" (v->counter));
#endif
}

/**
 *	atomic_sub - ԭ�ӵĽ�һ��atomic_t���ͱ�������һ��ָ����32λ������ֵ
 *
 *	@v:		ָ��atomic_t���͵�ָ��
 *	@i:		�����ֵ
 *
 *	remark:
 *		ԭ�ӵĽ�@v �� @i(����λ������)
 */
static INLINE_FN void atomic_sub(atomic_t *v, int i)
{
#ifdef WIN32
	InterlockedExchangeAdd((LPLONG)(&(v->counter)), -i);
#else
	 __asm__ __volatile__(
		LOCK "subl %1,%0"
		:"=m" (v->counter)
		:"ir" (i), "m" (v->counter));
#endif
}

/**
 *	atomic_inc - ԭ�ӵĽ�һ��atomic_t���ͱ�����һ
 *
 *	@v:		ָ��atomic_t���͵�ָ��
 *
 *	remark:
 *		ԭ�ӵĽ�@v �� 1(����λ������)
 */
static INLINE_FN void atomic_inc(atomic_t *v)
{
#ifdef WIN32
	InterlockedIncrement((LPLONG)(&(v->counter)));
#else
	__asm__ __volatile__(
		LOCK "incl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
#endif
}

/**
 *	atomic_dec - ԭ�ӵĽ�һ��atomic_t���ͱ�����һ
 *
 *	@v:		ָ��atomic_t���͵�ָ��
 *
 *	remark:
 *		ԭ�ӵĽ�@v �� 1(����λ������)
 */
static INLINE_FN void atomic_dec(atomic_t *v)
{
#ifdef WIN32
	InterlockedDecrement((LPLONG)(&(v->counter)));
#else
	__asm__ __volatile__(
		LOCK "decl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
#endif
}

/**
 *	atomic_add_return - ԭ�ӵĽ�һ��atomic_t���ͱ����� @i
 *
 *	@v:		ָ��atomic_t���͵�ָ��
 *	@i:		�����ֵ
 *
 *	remark:
 *		ԭ�ӵĽ�@v �� @i ͬʱԭ�ӵķ�����ӵ���ֵ(����λ������)
 */
static INLINE_FN int atomic_add_return(atomic_t *v, int i)
{
	int __i;

	__i = i;
#ifdef WIN32
	return InterlockedExchangeAdd((LPLONG)(&(v->counter)), i) + __i;
#else
	__asm__ __volatile__(
		LOCK "xaddl %0, %1"
		: "+r" (i), "+m" (v->counter)
		: : "memory");

	return i + __i;
#endif
}

/**
 *	atomic_inc_return - ԭ�ӵĽ�һ��atomic_t���ͱ����� 1
 *
 *	@v:		ָ��atomic_t���͵�ָ��
 *
 *	remark:
 *		ԭ�ӵĽ�@v �� 1 ͬʱԭ�ӵķ�����ӵ���ֵ(����λ������)
 */
static INLINE_FN int atomic_inc_return(atomic_t *v)
{
#ifdef WIN32
	return InterlockedIncrement((LPLONG)(&(v->counter)));
#else
	atomic_add_return(v, 1);
#endif
}

/**
 *	atomic_sub_return - ԭ�ӵĽ�һ��atomic_t���ͱ����� @i
 *
 *	@v:		ָ��atomic_t���͵�ָ��
 *	@i:		�����ֵ
 *
 *	remark:
 *		ԭ�ӵĽ�@v �� @i ͬʱԭ�ӵķ����������ֵ(����λ������)
 */
static INLINE_FN int atomic_sub_return(atomic_t *v, int i)
{
	return atomic_add_return(v, -i);
}

/**
 *	atomic_dec_return - ԭ�ӵĽ�һ��atomic_t���ͱ����� 1
 *
 *	@v:		ָ��atomic_t���͵�ָ��
 *
 *	remark:
 *		ԭ�ӵĽ�@v �� 1 ͬʱԭ�ӵķ����������ֵ(����λ������)
 */
static INLINE_FN int atomic_dec_return(atomic_t *v)
{
#ifdef WIN32
	return InterlockedDecrement((LPLONG)(&(v->counter)));
#else
	return atomic_sub_return(v, 1);
#endif
}

/**
 *	atomic_cmpxchg - ԭ�ӱȽ�atomic_t���ͱ�����cmp��ֵ
 *						��������ô��ô��atomic_t��ֵ
 *						���ó�chg��ֵ
 *
 *	@v:		ָ��atomic_t���͵�ָ��
 *	@chg:	����ֵ
 *	@cmp:	�Ƚ�ֵ
 *
 *	return
 *		����atomic_t���ͱ�����ԭ�е�ֵ
 *
 *	remark:
 *		�����CASָ��(����λ������)
 */
static INLINE_FN int atomic_cmpxchg(atomic_t *v, int chg, int cmp)
{
#ifdef WIN32
	return (int)InterlockedCompareExchange(
					(PVOID)(&(v->counter)), (PVOID)chg, (PVOID)cmp);
#else
	unsigned int	prev; 
	__asm__ __volatile__(
			LOCK "cmpxchgl %k1,%2"  
			: "=a"(prev)  
			: "r"(chg), "m"(*__xg((&(v->counter)))), "0"(cmp)  
			: "memory");  
	return prev;
#endif
}

#ifdef __cplusplus
}
#endif

#endif	/*	__ARCH_ATOMIC_I386_HEADER__*/

