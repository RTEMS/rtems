##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../console:@srcdir@/../../shared/vectors

C_FILES = vectors_init.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

S_FILES = vectors.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES) $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = $(top_srcdir)/../shared/vectors/vectors.h

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/bsp
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp

$(PROJECT_INCLUDE)/bsp/vectors.h: $(top_srcdir)/../shared/vectors/vectors.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/vectors.h

include $(top_srcdir)/../../../../automake/local.am
