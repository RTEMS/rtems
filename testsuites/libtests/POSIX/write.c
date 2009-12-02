/*
 * Copyright (c) 2009 by
 * Ralf Corsépius, Ulm, Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#include <unistd.h>

int
main (void)
{
  char string[] = "1234";
  size_t count = 4;
  ssize_t ret;

  ret = write (0, &string, count);

  return 0;
}
