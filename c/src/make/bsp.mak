#
# $Id$
#
# Makefile fragment to convert <bsp>.cfgs into <bsp>.cache
#

include make/custom/${RTEMS_BSP}.cfg

make/${RTEMS_BSP}.cache:
	@$(RM) $@
	@echo ac_cv_env_RTEMS_CPU_MODEL=\$${ac_cv_env_RTEMS_CPU_MODEL=$(RTEMS_CPU_MODEL)} >> $@
	@echo ac_cv_env_RTEMS_BSP_FAMILY=\$${ac_cv_env_RTEMS_BSP_FAMILY=$(RTEMS_BSP_FAMILY)} >> $@
	@echo ac_cv_env_CPU_CFLAGS=\$${ac_cv_env_CPU_CFLAGS=$(CPU_CFLAGS)} >> $@
	@echo ac_cv_env_CFLAGS_OPTIMIZE_V=\$${ac_cv_env_CFLAGS_OPTIMIZE_V=$(CFLAGS_OPTIMIZE_V)} >> $@
	@echo ac_cv_env_CFLAGS_DEBUG_V=\$${ac_cv_env_CFLAGS_DEBUG_V=$(CFLAGS_DEBUG_V)} >> $@
	@echo ac_cv_env_CFLAGS_PROFILE_V=\$${ac_cv_env_CFLAGS_PROFILE_V=$(CFLAGS_PROFILE_V)} >> $@
