/**
 * @file
 *
 * @ingroup ppc_exc
 *
 * @brief PowerPC Exceptions implementation.
 */

/*
 * Copyright (C) 1999 Eric Valette (valette@crf.canon.fr)
 *                    Canon Centre Recherche France.
 *
 * Derived from file "libcpu/powerpc/new-exceptions/bspsupport/vectors_init.c".
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp/vectors.h>

exception_handler_t globalExceptHdl = C_exception_handler;

void C_exception_handler(BSP_Exception_frame *excPtr)
{
  rtems_fatal(
    RTEMS_FATAL_SOURCE_POWERPC_EXCEPTION,
    (rtems_fatal_code) excPtr
  );
}
