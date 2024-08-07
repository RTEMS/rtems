SPARC
=====

This file discusses SPARC specific issues which are important to
this port.  The primary topics in this file are:

  + Global Register Usage
  + Stack Frame
  + EF bit in the PSR


Global Register Usage
---------------------

This information on register usage is based heavily on a comment in the
file gcc-2.7.0/config/sparc/sparc.h in the the gcc 2.7.0 source.

   + g0 is hardwired to 0
   + On non-v9 systems:
       - g1 is free to use as temporary.
       - g2-g4 are reserved for applications.  Gcc normally uses them as
         temporaries, but this can be disabled via the -mno-app-regs option.
       - g5 through g7 are reserved for the operating system.
   + On v9 systems:
       - g1 and g5 are free to use as temporaries.
       - g2-g4 are reserved for applications (the compiler will not normally use
         them, but they can be used as temporaries with -mapp-regs).
       - g6-g7 are reserved for the operating system.

   NOTE:  As of gcc 2.7.0 register g1 was used in the following scenarios:

       + as a temporary by the 64 bit sethi pattern
       + when restoring call-preserved registers in large stack frames

RTEMS places no constraints on the usage of the global registers.  Although 
gcc assumes that either g5-g7 (non-V9) or g6-g7 (V9) are reserved for the
operating system, RTEMS does not assume any special use for them.



Stack Frame
-----------

The stack grows downward (i.e. to lower addresses) on the SPARC architecture.

The following is the organization of the stack frame:

```

                |        ...............        |
             fp |                               |
                +-------------------------------+
                |                               |
                | Local registers, temporaries, |
                |      and saved floats         |      x bytes
                |                               |
        sp + x  +-------------------------------+
                |                               |
                |   outgoing parameters past    |
                |       the sixth one           |      x bytes
                |                               |
        sp + 92 +-------------------------------+  *
                |                               |  *
                |   area for callee to save     |  *
                |      register arguments       |  *  24 bytes
                |                               |  *
        sp + 68 +-------------------------------+  *
                |                               |  *
                |   structure return pointer    |  *   4 bytes
                |                               |  *
        sp + 64 +-------------------------------+  *
                |                               |  *
                |      local register set       |  *  32 bytes
                |                               |  *
        sp + 32 +-------------------------------+  *
                |                               |  *
                |      input register set       |  *  32 bytes
                |                               |  *
            sp  +-------------------------------+  *
```

* = minimal stack frame

x = optional components

EF bit in the PSR
=================

The EF (enable floating point unit) in the PSR is utilized in this port to
prevent non-floating point tasks from performing floating point
operations. This bit is maintained as part of the integer context. 
However, the floating point context is switched BEFORE the integer
context.  Thus the EF bit in place at the time of the FP switch may
indicate that FP operations are disabled.  This occurs on certain task
switches, when the EF bit will be 0 for the outgoing task and thus a fault
will be generated on the first FP operation of the FP context save. 

The remedy for this is to enable FP access as the first step in both the
save and restore of the FP context area.  This bit will be subsequently 
reloaded by the integer context switch.

Two of the scenarios which demonstrate this problem are outlined below:

1. When the first FP task is switched to.  The system tasks are not FP and
thus would be unable to restore the FP context of the incoming task. 

2. On a deferred FP context switch. In this case, the system might switch
from FP Task A to non-FP Task B and then to FP Task C.  In this scenario, 
the floating point state must technically be saved by a non-FP task. 
