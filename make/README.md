Make
====
This file describes the layout and conventions of the application
makefile support for RTEMS applications.  Internally, RTEMS uses
GNU-style autoconf/automake Makefiles as much as possible to 
ease integration with other GNU tools.

All of these "make" trees are substantially similar; however this
file documents the current state of the RTEMS Application Makefile
support.

This make tree is based on a build system originally developed
to simplify porting projects between various OS's.  The primary
goals were:

 .  simple *and* customizable individual makefiles

 .  use widely available GNU make.  There is no pre-processing or
	 automatic generation of Makefiles.

 .  Same makefiles work on *many* host OS's due to portability
	 of GNU make and the host OS config files.

 .  Support for different compilers and operating systems
	 on a per-user basis.  Using the same sources (including
	 Makefiles) one developer can develop and test under SVR4,
	 another under 4.x, another under HPUX.

 .  Builtin support for compiling "variants" such as debug
	 versions.  These variants can be built
	 recursively.

 .  Control of system dependencies.  "hidden" dependencies on
	 environment variables (such as PATH)
	 have been removed whenever possible.  No matter what your
	 PATH variable is set to, you should get the same thing
	 when you 'make' as everyone else on the project.

This Makefile system has evolved into its present form and as it
exists in RTEMS today, its sole goal is to build RTEMS applications.
The use of these Makefiles hides the complexity of producing
executables for a wide variety of embedded CPU families and target
BSPs.  Switching between RTEMS BSPs is accomplished via setting
the environment variable "RTEMS_MAKEFILE_PATH."

This description attempts to cover all aspects of the Makefile tree.  Most
of what is described here is maintained automatically by the configuration
files.

The example makefiles in make/Templates should be used as a starting
point for new directories.

There are 2 main types of Makefile:

    directory and leaf.


Directory Makefiles
-------------------

A Makefile in a source directory with sub-directories is called a
"directory" Makefile.

Directory Makefile's are simply responsible for acting as "middle-men"
and recursing into their sub-directories and propagating the make.

For example, directory src/bin will contain only a Makefile and
sub-directories.  No actual source code will reside in the directory.
The following commands:

```shell
    $ cd src/bin
    $ make all
```

would descend into all the subdirectories of 'src/bin' and recursively
perform a 'make all'.

A 'make debug' will recurse thru sub-directories as a debug build.

A template directory Makefile which should work in almost all
cases is in make/Templates/Makefile.dir


Leaf Makefiles
--------------

Source directories that contain source code for libraries or
programs use a "leaf" Makefile.

These makefiles contain the rules necessary to build programs
(or libraries).

A template leaf Makefile is in Templates/Makefile.leaf .  A template
leaf Makefile for building libraries is in Templates/Makefile.lib .


NOTE: To simplify nested makefile's and source maintenance, we disallow
combining source and directories (that make(1) would be expected to
recurse into) in one source directory.  Ie., a directory in the source
tree may contain EITHER source files OR recursive sub directories, but NOT
both.  This assumption is generally shared with GNU automake.

Variants (where objects go)
---------------------------

All binary targets are placed in a sub-directory whose name is (for
example):

    o-optimize/       -- optimized binaries
    o-debug/          -- debug binaries

Using the template Makefiles, this will all happen automatically. 
The contents of these directories are specific to a BSP.

Within a Makefile, the ${ARCH} variable is set to o-optimize,
o-debug, etc., as appropriate.

HISTORICAL NOTE: Prior to version 4.5, the name of the sub-directory
  in which objects were placed included the BSP name.
   
Typing 'make' will place objects in o-optimize.
'make debug' will place objects in o-debug.

The debug targets are equivalent to 'all' except that
CFLAGS and/or LDFLAGS are modified as per the compiler config file for
debug and profile support.

The targets debug etc., can be invoked recursively at
the directory make level.  So from the top of a tree, one could
install a debug version of everything under that point by:

```shell
    $ cd src/lib
    $ gmake debug
    $ gmake install
```

When building a command that is linked with a generated library, the
appropriate version of the library will be linked in.

For example, the following fragments link the normal, debug, or
version of "libmine.a" as appropriate:

```shell
    LD_LIBS   += $(LIBMINE)
    LIBMINE = ../libmine/${ARCH}/libmine.a

    ${ARCH}/pgm: $(LIBMINE) ${OBJS}
        $(make-exe)
```

If we do 'gmake debug', then the library in
../libmine/o-debug/libmine.a will be linked in.  If $(LIBMINE)
might not exist (or might be out of date) at this point, we could add

```shell
    ${LIBMINE}: FORCEIT
    cd ../libmine; ${MAKE} ${VARIANT_VA}
```

The above would generate the following command to build libmine.a:

```shell
    cd ../libmine; gmake debug
```

The macro reference ${VARIANT_VA} converts ${ARCH} to the word 'debug'
(in this example) and thus ensures the proper version of the library
is built.


Targets
-------

All Makefile's support the following targets:

    all 					-- make "everything"
    install 				-- install "everything"

The following targets are provided automatically by
the included config files:

    clean					-- delete all targets
    depend  				-- build a make dependency file
    "variant targets"		-- special variants, see below


All directory Makefiles automatically propagate all these targets.  If
you don't wish to support 'all' or 'install' in your source directory,
you must leave the rules section empty, as the parent directory Makefile
will attempt it on recursive make's.


Configuration
-------------

All the real work described here happens in file(s) included
from your Makefile.

All Makefiles include a customization file which is used to select
compiler and host operating system.  The environment variable
RTEMS_MAKEFILE_PATH must point to the directory containing this file; eg:

    	export RTEMS_MAKEFILE_PATH=/.../pc386/

All leaf Makefile's also include either 'make/leaf.cfg' (or
'make/lib.cfg' for building libraries).  These config files provide
default rules and set up the command macros as appropriate.

All directory Makefiles include 'make/directory.cfg'.  directory.cfg
provides all the rules for recursing through sub directories.

The Makefile templates already perform these include's.

'make/leaf.cfg' (or directory.cfg) in turn includes:

    a file specifying general purpose rules appropriate for
    	both leaf and directory makefiles.
    	( make/main.cfg )

    personality modules specified by the customization file for:
    	compiler			( make/compilers/??.cfg )


generic rules file
------------------

[ make/main.cfg ]
included by leaf.cfg or directory.cfg.

This file contains some standard rules and variable assignments
that all Makefiles need.

It also includes the FORCEIT: pseudo target.


OS config file for host machine
-------------------------------

[ make/os/OS-NAME.cfg ]
included by main.cfg

Figures out the target architecture and specifies command names
for the OS tools including RCS/CVS (but NOT for the compiler tools).


Compiler configuration for the target
-------------------------------------

[ compilers/COMPILER-NAME.cfg ]
included by leaf.cfg

Specifies the names of tools for compiling programs.
Names in here should be fully qualified, and NOT depend on $PATH.

Also specifies compiler flags to be used to generate optimized,
debugging versions, as well as rules to compile
assembly language and make makefile dependencies.


Configuration Variables
-----------------------

Variables you have to set in the environment or in your Makefile.
Note: the RTEMS module files set RTEMS_ROOT and RTEMS_CUSTOM
for you.

Makefile Variables
------------------

RTEMS_BSP        -- name of your 'bsp' eg: pc386, mvme136

RTEMS_CPU        -- CPU architecture e.g.: i386, m68k

RTEMS_CPU_FAMILY -- CPU model e.g.: i486dx, m68020

RTEMS_ROOT     -- The root of your source tree.
                  All other file names are derived from this.
                  [ eg: % setenv RTEMS_ROOT $HOME/work/RTEMS ]

RTEMS_CUSTOM   -- name of your config files in make/custom
                  Example:
                     $(RTEMS_ROOT)/make/custom/$(RTEMS_BSP).cfg

The value RTEMS_ROOT is used in the custom
files to generate the make(1) variables:

    PROJECT_RELEASE
    PROJECT_BIN
    PROJECT_INCLUDE
    PROJECT_TOOLS

etc., which are used within the make config files themselves.
(The files in make/*.cfg try to avoid use of word RTEMS so
they can be more easily shared by other projects)

Preset variables
----------------

Aside from command names set by the OS and compiler config files,
a number of MAKE variables are automatically set and maintained by
the config files.


PROJECT_RELEASE
            -- release/install directory
               [ $(PROJECT_ROOT) ]

PROJECT_BIN
            -- directory for installed binaries
               [ $(PROJECT_ROOT)/bin ]

PROJECT_TOOLS
            -- directory for build environment commands
               [ eg: $(PROJECT_ROOT)/build-tools ]

ARCH        -- target sub-directory for object code
               [ eg: o-optimize or o-debug ]

VARIANTS    -- full list of all possible values for $(ARCH);
               used mainly for 'make clean'
               [ eg: "o-optimize o-debug" ]

VARIANT_VA  -- Variant name.
               Normally "", but for 'make debug' it is "debug".

               see make/leaf.cfg for more info.
```

Preset compilation variables
----------------------------
 This is a list of some of the compilation variables.
 Refer to the compiler config files for the complete list.

```
   CFLAGS_OPTIMIZE_V   -- value of optimize flag for compiler
 						  [ eg: -O ]

   CFLAGS_DEBUG_V	   -- value of debug flag for compiler
 						  [ eg: -g ]

   CFLAGS_DEBUG
   CFLAGS_OPTIMIZE     -- current values for each depending
 						   on make variant.

   LDFLAGS_STATIC_LIBRARIES_V
 					   -- ld option for static libraries
 						   -Bstatic or -dy (svr4)

   LDFLAGS_SHARED_LIBRARIES_V
 					   -- ld option for dynamic libraries
 						   -Bdynamic or -dn (svr4)

Makefile Variables
------------------

The following variables may be set in a typical Makefile.

C_PIECES    -- File names of your .c files without '.c' suffix.
               [ eg: C_PIECES=main funcs stuff ]

CC_PIECES   -- ditto, except for .cc files

S_PIECES    -- ditto, except for .S files.

LIB         -- target library name in leaf library makefiles.
               [ eg: LIB=${ARCH}/libmine.a ]

H_FILES     -- your .h files in this directory.
               [ eg: H_FILES=stuff.h extra.h ]

DEFINES     -- cc -D items.  Included in CPPFLAGS.
               leaf Makefiles.
               [ eg: DEFINES += -DUNIX ]

CPPFLAGS    -- -I include directories.
               leaf Makefiles.
               [ eg: CPPFLAGS += -I../include ]

LD_PATHS    -- arguments to -L for ld.
               Will be prefixed with '-L' or '-L ' as appropriate
               and included in LDFLAGS.

LDFLAGS     -- -L arguments to ld; more may be ADDed.

LD_LIBS     -- libraries to be linked in.
               [ eg: LDLIBS += ../libfoo/${ARCH}/libfoo.a ]

XCFLAGS     -- "extra" CFLAGS for special needs.  Pre-pended
               to CFLAGS.
               Not set or used by Makefiles.
               Can be set on command line to pass extra flags
               to the compiler.

XCPPFLAGS   -- ditto for CPPFLAGS
               Can be set on command line to pass extra flags
               to the preprocessor.

XCCPPFLAGS  -- same as XCPPFLAGS for C++.

XCCFLAGS    -- same as XCFLAGS for C++.

SUBDIRS    -- list of sub directories for make recursion.
               directory Makefiles only.
               [ eg: SUBDIRS=cpu bsp ]

CLEAN_ADDITIONS
            -- list of files or directories that should
               be deleted by 'make clean'
               [ eg: CLEAN_ADDITIONS += y.tab.c ]

               See 'leaf.cfg' for the 'clean:' rule and its
               default deletions.

CLOBBER_ADDITIONS
            -- list of files or directories that should
               be deleted by 'make clobber'
               Since 'make clobber' includes 'make clean',
               you don't need to duplicate items in both.
```

Command names
-------------

The following commands should only be called
as make variables:

    MAKE,INSTALL,INSTALL_VARIANT,SHELL

    ECHO,CAT,CP,MV,LN,MKDIR,CHMOD

    SED

    CC,CPP,AS,AR,LD,NM,SIZE,RANLIB,MKLIB,
    YACC,LEX,LINT,CTAGS,ETAGS

 In addition, the following commands specifically support
 the installation of libraries, executables, header files,
 and other things that need to be installed:

    INSTALL_CHANGE  - set to host "install" program by default

    INSTALL_VARIANT - set to host "install" program by default


Special Directory Makefile Targets
----------------------------------

all_WRAPUP
clean_WRAPUP
install_WRAPUP
clean_WRAPUP
clobber_WRAPUP
depend_WRAPUP
            -- Specify additional commands for recursive
               (directory level) targets.

               This is handy in certain cases where you need
               to do bit of work *after* a recursive make.

make/Templates
--------------

This directory contains Makefile and source file templates that
should help in creating or converting makefiles.

Makefile.leaf
    Template leaf Makefiles.

Makefile.lib
    Template leaf library Makefiles.

Makefile.dir
    Template "directory" makefile.




