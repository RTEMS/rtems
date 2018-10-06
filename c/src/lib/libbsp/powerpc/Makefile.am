ACLOCAL_AMFLAGS = -I ../../../aclocal

## Descend into the @RTEMS_BSP_FAMILY@ directory
_SUBDIRS = @RTEMS_BSP_FAMILY@

# FIXME: This does not work
# DIST_SUBDIRS = @all_subdirs@ support/old_exception_processing \
#    support/new_exception_processing

# FIXME: Herein lies madness ;)

## shared/include

## shared/bootloader

## shared/clock

## shared/console

## shared/flash

## shared/motorola

## shared/pci

## shared/residual

## shared/openpic

## shared/irq

## shared/start

## shared/startup

## shared/tod

## shared/vme

include $(top_srcdir)/../../../automake/subdirs.am
include $(srcdir)/../../../../../bsps/powerpc/headers.am
