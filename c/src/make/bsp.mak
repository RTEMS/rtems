#
# $Id$
#
# Makefile fragment to convert <bsp>.cfgs into <bsp>.cache
#

include make/custom/${RTEMS_BSP}.cfg

make/${RTEMS_BSP}.cache:
	@$(RM) $@
	@echo rtems_cv_RTEMS_CPU_MODEL=\$${rtems_cv_RTEMS_CPU_MODEL=$(RTEMS_CPU_MODEL)} >> $@
	@echo rtems_cv_RTEMS_BSP_FAMILY=\$${rtems_cv_RTEMS_BSP_FAMILY=$(RTEMS_BSP_FAMILY)} >> $@
	@echo rtems_cv_RTEMS_BSP_CFLAGS=\$${rtems_cv_RTEMS_BSP_CFLAGS=$(CPU_CFLAGS)} >> $@
	@echo rtems_cv_RTEMS_BSP_CFLAGS_OPTIMIZE_V=\$${rtems_cv_RTEMS_BSP_CFLAGS_OPTIMIZE_V=$(CFLAGS_OPTIMIZE_V)} >> $@
	@echo rtems_cv_RTEMS_BSP_CFLAGS_DEBUG_V=\$${rtems_cv_RTEMS_BSP_CFLAGS_DEBUG_V=$(CFLAGS_DEBUG_V)} >> $@
	@echo rtems_cv_RTEMS_BSP_CFLAGS_PROFILE_V=\$${rtems_cv_RTEMS_BSP_CFLAGS_PROFILE_V=$(CFLAGS_PROFILE_V)} >> $@
