altivec
=======

Altivec support was developed and maintained as a user-extension
outside of RTEMS. This extension is still available (unbundled)
from Till Straumann <strauman@slac.stanford.edu>; it is useful
if an application desires 'lazy switching' of the altivec context.

Modes
-----

Altivec support -- the unbundled extension, that is -- can be used
in two ways:

a. All tasks are implicitly AltiVec-enabled.

b. Only designated tasks are AltiVec-enabled. 'Lazy-context switching'
   is implemented to switch AltiVec the context.

Note that the code implemented in this directory supports mode 'a'
and mode 'a' ONLY. For mode 'b' you need the unbundled extension
(which is completely independent of this code).

Mode 'a' (All tasks are AltiVec-enabled)
----------------------------------------

The major disadvantage of this mode is that additional overhead is 
involved: tasks that never use the vector unit still save/restore
the volatile vector registers (20 registers * 16bytes each) across
every interrupt and all non-volatile registers (12 registers * 16b each)
during every context switch.

However, saving/restoring e.g., the volatile registers is quite
fast -- on my 1GHz 7457 saving or restoring 20 vector registers
takes only about 1us or even less (if there are cache hits).

The advantage is complete transparency to the user and full ABI
compatibility (exept for ISRs and exception handlers), see below.

Mode 'b' (Only dedicated tasks are AltiVec-enabled)
---------------------------------------------------

The advantage of this mode of operation is that the vector-registers
are only saved/restored when a different, altivec-enabled task becomes
ready to run. In particular, if there is only a single altivec-enabled
task then the altivec-context *never* is switched.

Note that this mode of operation is not supported by the code
in this directory -- you need the unbundled altivec extension
mentioned above.

Compiler Options
----------------- 
```
Three compiler options affect AltiVec: -maltivec, -mabi=altivec and
-mvrsave=yes/no.

-maltivec: This lets the cpp define the symbol __ALTIVEC__ and enables
           gcc to emit vector instructions. Note that gcc may use the
           AltiVec engine implicitly, i.e., **without you writing any
           vectorized code**.

-mabi=altivec: This option has two effects:
           i) It ensures 16-byte stack alignment required by AltiVec
              (even in combination with eabi which is RTEMS' default).
           ii) It allows vector arguments to be passed in vector registers.

-mvrsave=yes/no: Instructs gcc to emit code which sets the VRSAVE register
           indicating which vector registers are 'currently in use'.
           Because the altivec support does not use this information *) the
           option has no direct affect but it is desirable to compile with
           -mvrsave=no so that no unnecessary code is generated.

          *) The file vec_sup_asm.S conditionally disables usage of
             the VRSAVE information if the preprocessor symbol
             'IGNORE_VRSAVE' is defined, which is the default.

             If 'IGNORE_VRSAVE' is undefined then the code *does*
             use the VRSAVE information but I found that this does
             not execute noticeably faster.
```

IMPORTANT NOTES
---------------

AFAIK, RTEMS uses the EABI which requires a stack alignment of only 8 bytes
which is NOT enough for AltiVec (which requires 16-byte alignment).

There are two ways for obtaining 16-byte alignment:

I)  Compile with -mno-eabi (ordinary SYSV ABI has 16-byte alignment)
II) Compile with -mabi=altivec (extension to EABI; maintains 16-byte alignment
    but also allows for passing vector arguments in vector registers)

Note that it is crucial to compile ***absolutely everything*** with the same
ABI options (or a linker error may occur). In particular, this includes

 - newlibc multilib variant
 - RTEMS proper 
 - application + third-party code

IMO the proper compiler options for Mode 'a' would be

    -maltivec -mabi=altivec -mvrsave=no

Note that the -mcpu=7400 option also enables -maltivec and -mabi=altivec
but leaves -mvrsave at some 'default' which is probably 'no'.
Compiling with -mvrsave=yes does not produce incompatible code but
may have a performance impact (since extra code is produced to maintain
VRSAVE).


Multilib Variants
-----------------

The default GCC configuration for RTEMS contains a -mcpu=7400 multilib
variant which is the correct one to choose.


BSP 'custom' file.
------------------
Now that you have the necessary newlib and libgcc etc. variants
you also need to build RTEMS accordingly.

In you BSP's make/custom/<bsp>.cfg file make sure the CPU_CFLAGS
select the desired variant:

for mode 'a':

```shell
   CPU_CFLAGS = ... -mcpu=7400
```

Note that since -maltivec globally defines __ALTIVEC__ RTEMS automatially
enables code that takes care of switching the AltiVec context as necessary.
This is transparent to application code.
6. BSP support
--------------
It is the BSP's responsibility to initialize MSR_VE, VSCR and VRSAVE
during early boot, ideally before any C-code is executed (because it
may, theoretically, use vector instructions).

The BSP must

 - set MSR_VE
 - clear VRSAVE; note that the probing algorithm for detecting
   whether -mvrsave=yes or 'no' was used relies on the BSP
   clearing VRSAVE during early start. Since no interrupts or
   context switches happen before the AltiVec support is initialized
   clearing VRSAVE is no problem even if it turns out that -mvrsave=no
   was in effect (eventually a value of all-ones will be stored
   in VRSAVE in this case).
 - clear VSCR

PSIM note
---------
PSIM supports the AltiVec instruction set with the exception of
the 'data stream' instructions for cache prefetching. The RTEMS
altivec support includes run-time checks to skip these instruction
when executing on PSIM.

Note that AltiVec support within PSIM must be enabled at 'configure'
time by passing the 'configure' option

```shell
--enable-sim-float=altivec
```

Note also that PSIM's AltiVec support has many bugs. It is recommended
to apply the patches filed as an attachment with gdb bug report #2461
prior to building PSIM.

The CPU type and corresponding multilib must be changed when
building RTEMS/psim:

  edit make/custom/psim.cfg and change

```shell
    CPU_CFLAGS = ... -mcpu=603e
```

  to

```shell
    CPU_CFLAGS = ... -mcpu=7400
```

This change must be performed *before* configuring RTEMS/psim.
