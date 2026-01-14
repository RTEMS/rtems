/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief libc needed by bootloaded
 *
 * This file contains the implementation of functions that are unresolved
 * in the bootloader.  Unfortunately it  shall not use any object code
 * from newlib or RTEMS because they are not compiled with the right option!!!
 *
 * @note You've been warned!!!.
 */

/*
 * Copyright (c) 1998, 1999 Eric Valette <eric.valette@free.fr>
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

/*
 * Provide our own prototypes to avoid warnings and risk getting inlined
 * conflicts from the normal header files.
 */
void* memset(void *p, int c, unsigned int n);
void* memcpy(void *dst, const void * src, unsigned int n);
char* strcat(char * dest, const char * src);
int strlen(const char* string);

void* memset(void *p, int c, unsigned int n)
{
  char *q =p;
  for(; n>0; --n) *q++=c;
  return p;
}

void* memcpy(void *dst, const void * src, unsigned int n)
{
  unsigned char *d=dst;
  const unsigned char *s=src;

  while(n-- > 0) *d++=*s++;
  return dst;
}

char* strcat(char * dest, const char * src)
{
  char *tmp = dest;

  while (*dest)
    dest++;
  while ((*dest++ = *src++) != '\0')
    ;
  return tmp;
}

int strlen(const char* string)
{
  register int i = 0;

  while (string[i] != '\0')
    ++i;
  return i;
}
