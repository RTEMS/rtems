## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

C_FILES = em86.c lib.c misc.c mm.c pci.c zlib.c

H_FILES = bootldr.h zlib.h pci.h
noinst_HEADERS = $(H_FILES)

S_FILES = em86real.S exception.S head.S

noinst_DATA = ppcboot.lds

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/lib.am

EXTRA_DIST = README $(C_FILES) $(S_FILES) $(noinst_DATA)

include $(top_srcdir)/../../../../../automake/local.am
