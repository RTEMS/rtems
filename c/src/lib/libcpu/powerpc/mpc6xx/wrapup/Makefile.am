##
## $Id$
##

# PROC_SPECIFIC_O_FILES = exceptions mmu clock
PROC_SPECIFIC_O_FILES = exceptions mmu
GENERIC_FILES =

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(PROC_SPECIFIC_O_FILES), \
    ../../mpc6xx/$(piece)/$(ARCH)/*.$(OBJEXT)) $(foreach piece, \
    $(GENERIC_FILES), ../../mpc6xx/$(piece)/$(ARCH)/*.$(OBJEXT))
LIB = $(ARCH)/libcpuspec.a

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): ${OBJS}
	echo $(OBJ)
	$(make-library)
	cp $(LIB) ..

all-local: ${ARCH} $(LIB)

include $(top_srcdir)/../../../automake/local.am
