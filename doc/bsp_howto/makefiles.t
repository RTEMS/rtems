@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Makefiles

@section Makefiles Used During The BSP Building Process

There's a makefile template in each directory of a BSP. They are called
"makefile.in" and are processed when building RTEMS for a given BSP. One
should specify the needed files and directories before the building
process. 

@itemize @bullet

@item the makefile.in at the BSP root specifies which folders have to be
included. For instance,

@item We only build the networking device driver if HAS_NETWORKING was defined

@example
NETWORKING_DRIVER_yes_V = network
NETWORKING_DRIVER = $(NETWORKING_DRIVER_$(HAS_NETWORKING)_V) 

[...]

SUB_DIRS=include start340 startup clock console timer \
    $(NETWORKING_DRIVER) wrapup
@end example

states that all the directories have to be processed, except for the
network directory which is included only if the user asked for it when
building RTEMS. 

@item the makefile.in in each driver directory. It lists the files to be
included in the driver, so don't forget to add the reference to a new file
in the makefile.in of a given driver when it is created! 

@end itemize


Rem : the makefile.in files are ONLY processed during the configure
process of a RTEMS build. It means that, when you're working on the design
of your BSP, and that you're adding a file to a folder and to the
corresponding makefile.in, it will not be take n into account! You have to
run configure again or modify the makefile (result of the makefile.in
process, usually in your <the RTEMS build
directory>/c/src/lib/libbsp/<your BSP family>/<your BSP>/<your driver>
directory) by hand. 

@section Makefiles Used Both During The BSP Design and its Use

A BSP must go with his configuration file. The configuration files can be
found under $RTEMS_ROOT/c/make/custom. The configuration file is taken
into account when building one's application using the template makefiles
($RTEMS_ROOT/c/make/templates), whic h is strongly advised. There are
templates for calling recursively the makefiles in the directories beneath
the current one, building a library or an executable.

The following is a hevaily commented version of the make customization
file for the gen68340 BSP.  It can be found in the $RTEMS_ROOT/make/custom
directory.

@example

# The RTEMS CPU Family and Model
RTEMS_CPU=m68k
RTEMS_CPU_MODEL=mcpu32

include $(RTEMS_ROOT)/make/custom/default.cfg

# The name of the BSP directory used for the actual source code.
# This allows for build variants of the same BSP source.
RTEMS_BSP_FAMILY=gen68340

# CPU flag to pass to GCC
CPU_CFLAGS = -mcpu32

# optimisation flag to pass to GCC
CFLAGS_OPTIMIZE_V=-O4 -fomit-frame-pointer

# The name of the start file to be linked with.  This file is the first
# part of the BSP which executes.
START_BASE=start340

[...]

# This make-exe macro is used in template makefiles to build the
# final executable. Any other commands to follow, just as using
# objcopy to build a PROM image or converting the executable to binary. 

ifeq ($(RTEMS_USE_GCC272),yes)
# This has rules to link an application if an older version of GCC is 
# to be used with this BSP.  It is not required for a BSP to support
# older versions of GCC.  This option is supported in some of the
# BSPs which already had this support.
[...]
else
# This has rules to link an application using gcc 2.8 or newer or any
# egcs version.  All BSPs should support this.  This version is required
# to support GNAT/RTEMS.
define make-exe
	$(CC) $(CFLAGS) $(CFLAGS_LD) -o $(basename $@@).exe $(LINK_OBJS)
	$(NM) -g -n $(basename $@@).exe > $(basename $@@).num
	$(SIZE) $(basename $@@).exe
endif
@end example



What you have to do:

@itemize @bullet

@item copy any .cfg file to <your BSP>.cfg

@item modify RTEMS_CPU, TARGET_ARCH, RTEMS_CPU_MODEL, RTEMS_BSP_FAMILY,
RTEMS_BSP, CPU_CFLAGS, START_BASE accordingly. 

@end itemize


