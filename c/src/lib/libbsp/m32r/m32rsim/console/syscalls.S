/*
 *  System call support for simulator in gdb.
 *  Adapted from newlib 1.16.0.
 */

#define SYS_exit        1
#define SYS_open        2
#define SYS_close       3
#define SYS_read        4
#define SYS_write       5
#define SYS_lseek       6
#define SYS_unlink      7
#define SYS_getpid      8
#define SYS_kill        9
#define SYS_fstat       10

#define POUND #
#define SYSCALL(N) mov.b POUND N,0x400

#define S(n) _sys_##n: .global _sys_##n | SYSCALL(SYS_##n) | rts

S(write)
S(exit)

       .global _abort
_abort:
        /* This is for debuggers.  The simulator stops here too. */
        brk

        /* Else, fall back on the simulator's "kill me" option.  */
#if defined(__r8c_cpu__) || defined(__m16c_cpu__)
        mov.w   #42,r1
#else
        mov.w   #42,r0
#endif

        SYSCALL(SYS_kill)

        /* Else, exit. */
        jmp.a   __exit

