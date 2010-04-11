/*
 * Copyright (c) 2009 by
 * Ralf Corsépius, Ulm, Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#include <stdio.h>

int main(void)
{
  FILE *file = NULL;
  flockfile(file);
  return 0;
}
