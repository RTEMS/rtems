/* lib.c
 *
 *  This file contains the implementation of functions that are unresolved
 *  in the bootloader.  Unfortunately it  shall not use any object code
 *  from newlib or rtems  because they are not compiled with the right option!!!
 *
 *  You've been warned!!!.
 *
 *  CopyRight (C) 1998, 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

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
