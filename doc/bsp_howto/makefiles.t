@chapter = Makefiles

@subsection = Makefiles Used During The BSP Building Process

There's a makefile template in each directory of a BSP. They are called
"makefile.in" and are processed when building RTEMS for a given BSP. One
should specify the needed files and directories before the building
process. 

@itemize @bullet

@item the makefile.in at the BSP root specifies which folders have to be
included. For instance,

@item We only build the ka9q device driver if HAS_KA9Q was defined

@example
KA9Q_DRIVER_yes_V = network
KA9Q_DRIVER = $(KA9Q_DRIVER_$(HAS_KA9Q)_V) 

[...]

SUB_DIRS=include start340 startup clock console timer $(KA9Q_DRIVER) wrapup
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

@subsection = Makefiles Used Both During The BSP Design and its Use

A BSP must go with his configuration file. The configuration files can be
found under $RTEMS_ROOT/c/make/custom. The configuration file is taken
into account when building one's application using the template makefiles
($RTEMS_ROOT/c/make/templates), whic h is strongly advised. There are
templates for calling recursively the makefiles in the directories beneath
the current one, building a library or an executable.

Let's see what it looks like :

@example
RTEMS_CPU=m68k

TARGET_ARCH=o-gen68340
RTEMS_CPU_MODEL=mcpu32

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_BSP_FAMILY=gen68340

## Target compiler config file, if any
CONFIG.$(TARGET_ARCH).CC = $(RTEMS_ROOT)/make/compilers/gcc-target-default.cfg

RTEMS_BSP=gen68340

CPU_CFLAGS = -mcpu32

# optimize flag: 
CFLAGS_OPTIMIZE_V=-O4 -fomit-frame-pointer

# Override default start file
START_BASE=start340

[...]

ifeq ($(RTEMS_USE_GCC272),yes)
[...]
else

define make-exe
	$(CC) $(CFLAGS) $(CFLAGS_LD) -o $(basename $@).exe $(LINK_OBJS)
	$(NM) -g -n $(basename $@).exe > $(basename $@).num
	$(SIZE) $(basename $@).exe
endif
@end example

the main board CPU family

directory where you want the objects to go

the main board CPU name for GCC

include standard definitions

bsp directory used during the build process 

cross-compiler options




the name of your bsp directory

CPU flag to pass to GCC

optimisation flag to pass to GCC


name of your BSP function to be called immediately after reboot



if an older version of GCC is used...



this macro is used in template makefiles to build the final executable.
Any other commands to follow, just as using objcopy to build a PROM image
or converting the executable to binary. 

Figure 1 : custom file for gen68340 BSP ($RTEMS_ROOT/make/custom/gen68340.cfg)



What you have to do:

@itemize @bullet

@item copy any .cfg file to <your BSP>.cfg

@item modify RTEMS_CPU, TARGET_ARCH, RTEMS_CPU_MODEL, RTEMS_BSP_FAMILY,
RTEMS_BSP, CPU_CFLAGS, START_BASE accordingly. 

@end itemize


