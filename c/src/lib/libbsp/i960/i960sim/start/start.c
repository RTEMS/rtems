char *__env[1] = {0};
char **environ = __env;

#define ENABLE_TRACE_MASK 1
#define STACK_ALIGN 64

__inline static void 
init_Cregs (void)
{
    /* set register values gcc like */
    register unsigned int mask0=0x3b001000;
    register unsigned int mask1=0x00009107;
    __asm__ volatile ("mov   %0,g14"
                      :                      /* no output */
                      : "I" (0));            /* gnu structure pointer */
    __asm__ volatile ("modac %1,%0,%0"
                      :                      /* no output */
                      : "d" (mask0),
                        "d" (mask1));        /* fpu control kb */
}

void _start(void)
{
  extern int stack_init;
  register void *ptr = &stack_init;

  /* enable tracing */
  register int mask = ENABLE_TRACE_MASK;
  __asm__ volatile ("modpc %0,%0,%0" : : "d" (mask));

  /* SP must be 64 bytes larger than FP at start.  */
  __asm__ volatile ("mov %0,sp" : : "d" (ptr + STACK_ALIGN));
  __asm__ volatile ("mov %0,fp" : : "d" (ptr));


  init_Cregs();
  boot_card();
  _sys_exit(0);
}
