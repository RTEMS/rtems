/**
 * @ingroup bsp_epiphany
 *
 * @brief Epiphany BSP Options
 */

/*
 *
 * Copyright (c) 2015 University of York.
 * Hesham ALMatary <hmka501@york.ac.uk>
 *
 * COPYRIGHT (c) 1989-1999.
 * On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* include/bspopts.h.in.  Generated from configure.ac by autoheader.  */

#ifndef LIBBSP_EPIPHANY_EPIPHANY_SIM_OPTS_H
#define LIBBSP_EPIPHANY_EPIPHANY_SIM_OPTS_H

/**
 * @name Options
 *
 * @{
 */

/**
 * @brief Dirties memory to test for programs that assume zeroed memory.
 *
 * If defined, then the BSP Framework will put a non-zero pattern into the
 * RTEMS Workspace and C program heap. This should assist in finding code that
 * assumes memory starts set to zero.
 */
#undef BSP_DIRTY_MEMORY

/**
 * @brief Print and wait for key press for reset.
 *
 * If defined, print a message and wait until pressed before resetting board
 * when application exits.
 */
#undef BSP_PRESS_KEY_FOR_RESET

/**
 * @brief Prints exception context upon unexpected exceptions
 *
 * If defined, prints the exception context when an unexpected exception occurs.
 */
#undef BSP_PRINT_EXCEPTION_CONTEXT

/**
 * @brief Resets on application exit.
 *
 * If defined, reset the board when the application exits.
 */
#undef BSP_RESET_BOARD_AT_EXIT

/**
 * @brief Reset vector address for BSP start
 */
#undef BSP_START_RESET_VECTOR

/**
 * @brief Define to the address where bug reports for this package should be
 * sent.
 */
#undef PACKAGE_BUGREPORT

/**
 * @brief Define to the full name of this package.
 */
#undef PACKAGE_NAME

/**
 * @brief Define to the full name and version of this package.
 */
#undef PACKAGE_STRING

/**
 * @brief Define to the one symbol short name of this package.
 */
#undef PACKAGE_TARNAME

/**
 * @brief Define to the home page for this package.
 */
#undef PACKAGE_URL

/**
 * @brief Define to the version of this package.
 */
#undef PACKAGE_VERSION

/** @} */

#endif /* LIBBSP_EPIPHANY_EPIPHANY_SIM_OPTS_H */
