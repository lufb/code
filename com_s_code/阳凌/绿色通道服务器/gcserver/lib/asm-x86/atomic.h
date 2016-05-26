/*
 *	include/asm-x86/atomic.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	基于x86体系结构的原子操作一些函数
 *
 *	在任何处理器平台下,都会有一些原子性操作,供操作给我们程序使用
 *	但是可惜的是这个部分通常不会用C语言的方式来提供,幸运的是我们
 *	
 *	WINDOWS平台下可以使用InterlockedFunction族函数实现原子操作
 *	无需要考虑多CPU的情况这个由InterlockedFunction族函数解决
 *	
 *	在LINUX平台我们可以使用嵌入式汇编实现原子的操作,值得注意的是
 *	LINUX下编译的时候最好确认目标程序要运行的环境是否是多处理情况
 *	在单处理器情况下,每条指令的执行都是原子性的
 *	但在多处理器情况下只有那些单独的读操作或写操作才是原子性的.
 *	为了弥补这一缺点,x86汇编指令提供了附加的lock前缀,
 *	使带lock前缀的读修改写指令也能原子性执行 带lock前缀的指令在操作时会锁住总线
 *	使自身的执行即使在多处理器间也是原子性执行的 
 *	我参考了CPU的指令说明,上面提到xchg指令不带lock前缀也是原子性执行
 *	也就是说xchg执行时默认会锁内存总线.
 *	但是为了安全期间,我还是在xchg指令加了LOCK前缀的.所以需要运行在多CPU的话
 *	LINUX的编译需要加上 -D CONFIG_SMP宏来处理
 *
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef	__ARCH_ATOMIC_I386_HEADER__
#define	__ARCH_ATOMIC_I386_HEADER__

#include	"features.h"

#ifdef WIN32
#else
#ifdef	CONFIG_SMP						/*	对称多CPU的前缀*/
#define LOCK "lock ; "
#else
#define LOCK ""
#endif
#endif

/*	基与I386的原子操作类型*/
#ifdef WIN32
typedef struct {ULONG volatile counter;} atomic_t;
#else
typedef struct {volatile int counter;} atomic_t;
#endif

/**
 *	atomic_set - 原子设置变量
 *
 *	@v:		指向atomic_t类型的指针
 *
 *	remark:
 *		返回原子读到的&v值(符号位被忽略)
 */
#define atomic_read(v)      ((v)->counter)

#ifdef  __cplusplus
extern  "C" {
#endif

/**
 *	atomic_set - 原子设置变量
 *
 *	@v:		指向atomic_t类型的指针
 *	@i:		请求的值
 *
 *	remark:
 *		原子的设置值 @i --> @v (符号位被忽略)
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
 *	atomic_add - 原子的将一个atomic_t类型变量增加一个指定的32位的整性值
 *
 *	@v:		指向atomic_t类型的指针
 *	@i:		请求的值
 *
 *	remark:
 *		原子的将@v 加 @i(符号位被忽略)
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
 *	atomic_sub - 原子的将一个atomic_t类型变量减少一个指定的32位的整性值
 *
 *	@v:		指向atomic_t类型的指针
 *	@i:		请求的值
 *
 *	remark:
 *		原子的将@v 减 @i(符号位被忽略)
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
 *	atomic_inc - 原子的将一个atomic_t类型变量加一
 *
 *	@v:		指向atomic_t类型的指针
 *
 *	remark:
 *		原子的将@v 加 1(符号位被忽略)
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
 *	atomic_dec - 原子的将一个atomic_t类型变量减一
 *
 *	@v:		指向atomic_t类型的指针
 *
 *	remark:
 *		原子的将@v 减 1(符号位被忽略)
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
 *	atomic_add_return - 原子的将一个atomic_t类型变量加 @i
 *
 *	@v:		指向atomic_t类型的指针
 *	@i:		请求的值
 *
 *	remark:
 *		原子的将@v 加 @i 同时原子的返回相加的新值(符号位被忽略)
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
 *	atomic_inc_return - 原子的将一个atomic_t类型变量加 1
 *
 *	@v:		指向atomic_t类型的指针
 *
 *	remark:
 *		原子的将@v 加 1 同时原子的返回相加的新值(符号位被忽略)
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
 *	atomic_sub_return - 原子的将一个atomic_t类型变量减 @i
 *
 *	@v:		指向atomic_t类型的指针
 *	@i:		请求的值
 *
 *	remark:
 *		原子的将@v 减 @i 同时原子的返回相减的新值(符号位被忽略)
 */
static INLINE_FN int atomic_sub_return(atomic_t *v, int i)
{
	return atomic_add_return(v, -i);
}

/**
 *	atomic_dec_return - 原子的将一个atomic_t类型变量减 1
 *
 *	@v:		指向atomic_t类型的指针
 *
 *	remark:
 *		原子的将@v 减 1 同时原子的返回相减的新值(符号位被忽略)
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
 *	atomic_cmpxchg - 原子比较atomic_t类型变量与cmp的值
 *						如果相等那么那么将atomic_t的值
 *						设置成chg的值
 *
 *	@v:		指向atomic_t类型的指针
 *	@chg:	交换值
 *	@cmp:	比较值
 *
 *	return
 *		返回atomic_t类型变量中原有的值
 *
 *	remark:
 *		经典的CAS指令(符号位被忽略)
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

