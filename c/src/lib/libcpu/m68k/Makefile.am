ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

noinst_PROGRAMS =

if mcf548x
## mcf548x/include
## mcf548x/mcdma
noinst_PROGRAMS += mcf548x/mcdma.rel
mcf548x_mcdma_rel_SOURCES = mcf548x/mcdma/MCD_dmaApi.c mcf548x/mcdma/MCD_tasksInit.c \
                            mcf548x/mcdma/MCD_tasks.c  mcf548x/mcdma/mcdma_glue.c

mcf548x_mcdma_rel_CPPFLAGS = $(AM_CPPFLAGS)
mcf548x_mcdma_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(top_srcdir)/../../../automake/local.am
