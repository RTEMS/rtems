/*
 * Copyright (c) 2009 by
 * Ralf Corsépius, Ulm, Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#include <sys/types.h>
#include <sys/wait.h>

int
main (void)
{
  int status;
  pid_t pid;
  pid = waitpid (-1, &status, WNOHANG);

  return 0;
}
