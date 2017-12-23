#ifndef _LIBCPU_STACKTRACE_H
#define _LIBCPU_STACKTRACE_H

void CPU_stack_take_snapshot(void **stack, int size, void *pc, void *lr, void *r1);

void CPU_print_stack(void);

#endif
