/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef __FSTEST_H
#define __FSTEST_H

#define TIME_PRECISION  (2)
#define TIME_EQUAL(x,y) (abs((x)-(y))<TIME_PRECISION)


#define FS_PASS() do {puts("PASS");} while (0)
#define FS_FAIL() do {\
  printf( "FAIL   %s: %d \n", __FILE__, __LINE__ );\
 } while (0)


#define SHOW_MESSAGE(e, func, ...) printf(\
    "Testing %-10s with arguments: %-20s EXPECT %s\n",\
    #func,#__VA_ARGS__,#e)

#define EXPECT_EQUAL(expect, function, ...)  do { \
  SHOW_MESSAGE(#expect,function,__VA_ARGS__);\
 if (expect==function(__VA_ARGS__)) \
     FS_PASS();\
 else \
     FS_FAIL();\
   } while (0)

#define EXPECT_UNEQUAL(expect, function, ...)  do { \
  SHOW_MESSAGE(#expect,function,__VA_ARGS__);\
 if (expect!=function(__VA_ARGS__)) \
     FS_PASS();\
 else\
     FS_FAIL();\
   } while (0)

#define EXPECT_ERROR(ERROR, function, ...)  do { \
  SHOW_MESSAGE(#ERROR,function,#__VA_ARGS__);\
 if ((-1==function(__VA_ARGS__)) && (errno==ERROR)) \
     FS_PASS();\
 else \
     FS_FAIL();\
   } while (0)

void test(void);

#define BASE_FOR_TEST "/mnt"
#endif
