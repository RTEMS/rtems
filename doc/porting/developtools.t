@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Development Tools

When porting RTEMS to a new CPU architecture, one will have to have a
development environment including compiler, assembler, linker, and
debugger.  The GNU development tool suite used by RTEMS supports most
modern CPU families.  Often all that is required is to add RTEMS
configurations for the target CPU family.  RTEMS targets for the GNU tools
usually start life as little more than aliases for existing embedded
configurations.  At this point in time, ELF is supported on most of the
CPU families with a tool target of the form CPU-elf.  If this target is
not supported by all of the GNU tools, then it will be necessary to
determine the configuration that makes the best starting point regardless
of the target object format.

Porting and retargetting the GNU tools is beyond the scope of this manual.  
The best advice that can be offered is to look at the existing RTEMS
targets in the tool source and use that as a guideline.

