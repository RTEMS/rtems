##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

#
# First and second generation used different Bridge chips :(
#       Generation 1 --> 82378zb (now in the CVS Attic)
#       Generation 2 --> FPGA
STARTUP_C_FILES = FPGA.c

C_FILES = bspclean.c bsplibc.c bsppost.c bspstart.c bootcard.c main.c sbrk.c \
    setvec.c Hwr_init.c spurious.c genpvec.c $(STARTUP_C_FILES) \
    gnatinstallhandler.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

project_lib_DATA = linkcmds

all-local: $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = FPGA.c Hwr_init.c bspclean.c bspstart.c genpvec.c linkcmds \
    setvec.c spurious.c vmeintr.c

${PROJECT_RELEASE}/lib/$(dirstamp):
	@$(mkdir_p) ${PROJECT_RELEASE}/lib
	@: > ${PROJECT_RELEASE}/lib/$(dirstamp)

${PROJECT_RELEASE}/lib/linkcmds: linkcmds ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/linkcmds
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/linkcmds

include $(top_srcdir)/../../../../automake/local.am
