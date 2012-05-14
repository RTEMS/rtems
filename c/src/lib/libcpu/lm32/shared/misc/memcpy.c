/*
 *  C library memcpy routine
 *
 *  This routine shall get code to optimize performance on NIOS II
 *
 *  The routine is placed in this source directory to ensure that it
 *  is picked up by all applications.
 */

#include <string.h>

void *
memcpy(void *s1, const void *s2, size_t n)
{
  char *p1 = s1;
  const char *p2 = s2;
  size_t left = n;

  while(left > 0) *(p1++) = *(p2++);
    return s1;
}
