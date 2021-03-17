/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Defines NEWLIB_HAS_LONG_DOUBLE_MATH_FUNCTIONS
 *
 * This file uses logic copied from newlib's math.h on 21 Jan 2021
 * to determine whether or not the long double math functions tests
 * should be built. The original math.h file was sourced from
 * https://codeload.github.com/RTEMS/sourceware-mirror-newlib-cygwin/tar.gz/a0d7982
 * The original math.h this is based on has no license or copyright notice
 */

/*
 * Copyright (C) 2021 OAR Corporation.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef HAS_LONG_DOUBLE_H
#define HAS_LONG_DOUBLE_H

/* Newlib doesn't fully support long double math functions so far.
   On platforms where long double equals double the long double functions
   simply call the double functions.  On Cygwin the long double functions
   are implemented independently from newlib to be able to use optimized
   assembler functions despite using the Microsoft x86_64 ABI. */
#if defined (_LDBL_EQ_DBL) || defined (__CYGWIN__)
#ifndef __math_68881
#define NEWLIB_HAS_LONG_DOUBLE_MATH_FUNCTIONS 1
#endif
#endif

#endif
