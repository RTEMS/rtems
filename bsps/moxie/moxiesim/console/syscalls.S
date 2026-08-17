/*
 *  System call support for simulator in gdb.
 *
 *  COPYRIGHT (c) 2011 Anthony Green <green@moxielogic.com>
 */

#define SYS_exit        1
#define SYS_write       5

        .section .text
        .align 2
        .global _sys_write
_sys_write:
        swi     SYS_write
        ret

/*
 *  Terminate the simulator.  The exit status is passed in $r0.
 */
        .align 2
        .global _sys_exit
_sys_exit:
        swi     SYS_exit
        ret
        .end
