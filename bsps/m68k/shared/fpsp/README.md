M68040FPSP Floating Point
=========================

Motorola 68040 floating point support package

Modified for RTEMS by Eric Norum (eric@norum.ca)

To include these routines in your application call

	M68KFPSPInstallExceptionHandlers ();

before performing any floating point operations.


Acknowledgement
---------------

This code can be obtain from the Motorola Engineer's Toolbox WWW page
at http://www.mot.com/SPS/HPESD/tools/freeware/040fpsp.html.  Here is
the description from that page:

  The MC68040 contains a subset of the floating-point hardware that is
  implemented in the MC68881/882 devices and as such provides reduced yet
  high performance on-chip floating-point support. Those applications that
  require full compatibility with earlier members of the M68000 family
  will need to provide emulation support fo r the un-implemented MC68040
  floating-point instructions. The M68040FPSP provides complete emulation
  of the floating-point functionality available in the MC68881/882. 

  The M68040FPSP is offered in source code form to allow integration into
  existing systems to support either a kernel or library version of
  floating-point support. The M68040FPSP operates in conjunction with the
  on-chip MC68040 features to provide fast and full emulation. The kernel
  version allows full emulation via a trap mechanism to allow full binary
  compatibility and is fully reentrant. The library version is used to
  eliminate the trap overhead in situation where re-compilation is
  possible or desired. 

From this page one may download the original source code.  Inline with
the first sentence of the second paragraph, we have integrated it with
RTEMS.
