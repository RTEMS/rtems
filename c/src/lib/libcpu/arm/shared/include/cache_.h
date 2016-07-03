/**
 * @file
 *
 * @ingroup arm
 *
 * @brief ARM cache dummy include for chips without cache
 */

/*
 * Copyright (c) 2009-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBCPU_ARM_CACHE__H
#define LIBCPU_ARM_CACHE__H

/*
 * The ARM targets equipped by cache should include
 * which kind and implementation they support.
 * Next options are available
 *
 * c/src/lib/libbsp/arm/shared/armv467ar-basic-cache/cache_.h
 *   basic ARM cache integrated on the CPU core directly
 *   which requires only CP15 oparations
 *
 * c/src/lib/libbsp/arm/shared/arm-l2c-310/cache_.h
 *   support for case where ARM L2C-310 cache controller
 *   is used. It is accessible as mmaped peripheral.
 *
 * c/src/lib/libbsp/arm/shared/armv7m/include/cache_.h
 *   Cortex-M specific cache support
 *
 * Cache support should be included in BSP Makefile.am
 *
 * Example how to include cache support
 *
 * # Cache
 * libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
 * libbsp_a_SOURCES += ../shared/include/arm-cache-l1.h
 * libbsp_a_SOURCES += ../shared/armv467ar-basic-cache/cache_.h
 * libbsp_a_CPPFLAGS += -I$(srcdir)/../shared/armv467ar-basic-cache
 */

#if defined(__ARM_ARCH_5TEJ__) || defined(__ARM_ARCH_7A__)
#warning ARM 5TEJ and ARMv7/Cortex-A cores include usually cache
#warning change BSP to include appropriate cache implementation
#endif

#endif /* LIBCPU_ARM_CACHE__H */
