#ifndef CPU_STACK_UTIL_H
#define CPU_STACK_UTIL_H

void CPU_stack_take_snapshot(void **stack, int size, void *pc, void *lr, void *r1);

void CPU_print_stack(void);

#endif
