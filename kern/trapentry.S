/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
  https://pdos.csail.mit.edu/6.828/2018/readings/ia32/IA32-3A.pdf#page=6&zoom=240,64,658

 */


TRAPHANDLER_NOEC(T_DIVIDE_H, T_DIVIDE)
TRAPHANDLER_NOEC(T_DEBUG_H, T_DEBUG)
TRAPHANDLER_NOEC(T_NMI_H, T_NMI)
TRAPHANDLER_NOEC(T_BRKPT_H, T_BRKPT)
TRAPHANDLER_NOEC(T_OFLOW_H, T_OFLOW)
TRAPHANDLER_NOEC(T_BOUND_H, T_BOUND)
TRAPHANDLER_NOEC(T_ILLOP_H, T_ILLOP)
TRAPHANDLER_NOEC(T_DEVICE_H, T_DEVICE)
TRAPHANDLER(T_DBLFLT_H, T_DBLFLT)
TRAPHANDLER(T_TSS_H, T_TSS)
TRAPHANDLER(T_SEGNP_H, T_SEGNP)
TRAPHANDLER(T_STACK_H, T_STACK)
TRAPHANDLER(T_GPFLT_H, T_GPFLT)
TRAPHANDLER(T_PGFLT_H, T_PGFLT)
TRAPHANDLER_NOEC(T_FPERR_H, T_FPERR)
TRAPHANDLER(T_ALIGN_H, T_ALIGN)
TRAPHANDLER_NOEC(T_MCHK_H, T_MCHK)
TRAPHANDLER_NOEC(T_SIMDERR_H, T_SIMDERR)
TRAPHANDLER_NOEC(T_SYSCALL_H, T_SYSCALL)
TRAPHANDLER_NOEC(T_DEFAULT_H, T_DEFAULT)



/*
 * Lab 3: Your code here for _alltraps
 */


_alltraps:
	 pushl %ds
	 pushl %es
	 pushal
	 pushl $GD_KD
	 pushl $GD_KD
	 popl %es
	 popl %ds

	 pushl %esp
	 call trap
