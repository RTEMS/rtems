#include "stackTrace.h"
#include <rtems/bspIo.h>
#include <libcpu/spr.h>

SPR_RO(PPC_LR)

typedef struct FrameRec_ {
	struct FrameRec_ *up;
	void		 *lr;
} FrameRec, *Frame;

#define CPU_STACK_TRACE_DEPTH 40

void CPU_stack_take_snapshot(void **stack, int size, void *pc, void *lr, void *r1)
{
register Frame	p = (Frame)lr;
register int	i=0;
	if (pc) stack[i++]=pc;
	if (!p)
		p = (Frame)_read_PPC_LR();
	stack[i++]=p;
	p = r1;
	if (!p) /* no macro for reading user regs */
		__asm__ __volatile__("mr %0, %%r1":"=r"(p));
	for (; i<size-1 && p->up; p=p->up, i++) {
		stack[i]=p->up->lr;
	}
	stack[i]=0;
}

void CPU_print_stack(void)
{
	void *stck[CPU_STACK_TRACE_DEPTH];
	int i;
	CPU_stack_take_snapshot(stck,CPU_STACK_TRACE_DEPTH,0,0,0);
        for (i=0; stck[i]; i++) {
                if (i%5) printk("--> ");
                else     printk("\n");
                printk("0x%08x",stck[i]);
	}
	printk("\n");
}
