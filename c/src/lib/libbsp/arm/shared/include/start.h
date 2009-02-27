/**
 * @file
 *
 * @brief Start entry functions.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_START_H
#define LIBBSP_ARM_SHARED_START_H

#ifndef ASM

  /**
   * @brief System start entry.
   */
  void start( void);

  /**
   * @brief Start entry hook 0.
   *
   * This hook will be called from the start entry code after all modes and
   * stack pointers are initialized but before the copying of the exception
   * vectors.
   */
  void bsp_start_hook_0( void);

  /**
   * @brief Start entry hook 1.
   *
   * This hook will be called from the start entry code after copying of the
   * exception vectors but before the call to boot card.
   */
  void bsp_start_hook_1( void);

#else

  .extern bsp_start_hook_0

  .extern bsp_start_hook_1

#endif

#endif /* LIBBSP_ARM_SHARED_START_H */
